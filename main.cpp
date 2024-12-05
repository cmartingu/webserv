#include "webserv.hpp"

int handle_error(int fd, std::string a)
{
	std::cerr << "Error en '" << a << "': " << strerror(errno) << std::endl;
	close(fd);
	return -1;
}

int setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);   // Obtiene los flags actuales del socket
	if (flags == -1)
		return handle_error(fd, "get flags");

	// Configura el socket para que sea no bloqueante
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return handle_error(fd, "set non-blocking");

	return 0;
}

bool readUntilContentLength(int socket, std::string& fullRequest, int contentLength)
{
    const int bufferSize = 1024;  // Tamaño del búfer
    char buffer[bufferSize];
    int bytesRead = 0;

    // Continuar leyendo hasta que hayamos recibido contentLength bytes
    while (bytesRead < contentLength) {
        int n = recv(socket, buffer, std::min(bufferSize, contentLength - bytesRead), 0);
        if (n <= 0) {
            return false; // Error o conexión cerrada
        }

        fullRequest.append(buffer, n);
        bytesRead += n;
    }

    return true;
}

int ft_stoi(std::string str)
{
	int result = 0;
	std::stringstream(str) >> result;
	return result;
}

std::string	ft_recv(int fd)
{
	std::string	fullRequest = "";
	std::string	buffer(1024, '\0');
	bool 		finished = false;
	int			ct_len = 0;

	while (!finished)
	{
		std::fill(buffer.begin(), buffer.end(), '\0');
		int bytesRead = recv(fd, &buffer[0], buffer.size(), 0);
		if (bytesRead > 0)
		{
			fullRequest += buffer;
			size_t n = buffer.find("Content-Length: ");
			if (n == std::string::npos)
				return fullRequest;
			else
			{
				size_t end = buffer.find('\n', n);
				std::string ct_len_aux = buffer.substr(n + 16, end - n - 16);
				ct_len = ft_stoi(ct_len_aux);
			}
		}
		else
			return fullRequest;
	}
	return fullRequest;
}

int	main(int argc, char **argv)
{
	int server_fd;
	int new_socket;
	std::string buffer(1024, '\0');
	socklen_t addrlen; // = sizeof(address);
	std::vector<pollfd> fds;

	if (argc != 2)
	{
		std::cerr << "Error: invalid number of arguments" << std::endl;
		return (1);
	}
	try
	{
		Config conf(argv[1]);  //Comprobar que la IP es valida, y sino pasarle IN_ADDR_ANY
		conf.saveServers(); // ya está todo guardado perfectamente

		int	max_fd = 0;

		for (int i = 0; i < conf.getNbServers(); i++)
		{
			Server serv = *conf.getServer(i);
			server_fd = serv.createServerSocket();
			if (server_fd == -1)
				return -1;
			if (setNonBlocking(server_fd) == -1)
				return -1;
			pollfd server;
			server.fd = server_fd;
			server.events = POLLIN;  // Monitorear nuevas conexiones
			fds.push_back(server);
			if (server.fd > max_fd)
				max_fd = server.fd;
		}

		while (true)
		{
			int ret = poll(fds.data(), fds.size(), 1000); // Esperar indefinidamente por eventos
			if (ret == -1)
			{
				std::cerr << "Error en poll: " << strerror(errno) << std::endl;
				break; //Salir del bucle si hay error
			}
			for (size_t i = 0; i < fds.size(); ++i) //Empieza en 1 porque el 0 es el servidor
			{
				if (fds[i].revents & POLLIN)
				{
					std::cout << "----- Entrada de algo -------" << std::endl;
					if ((int)i < conf.getNbServers()) //Sustituir con numero de ports donde lo tengamos
					{
						sockaddr_in client_addr;
						addrlen = sizeof(client_addr);
						new_socket = accept(fds[i].fd, (struct sockaddr *)&client_addr, &addrlen);
						if (new_socket == -1)
						{
							std::cerr << "Error en accept: " << strerror(errno) << std::endl;
							continue; // Continuar con la siguiente iteración del bucle
						}
						std::cout << "Nueva conexión aceptada en el socket " << new_socket << std::endl;
						if (setNonBlocking(new_socket))
							continue; //Nueva iteracion del bucle
						pollfd new_client;
						new_client.fd = new_socket; // Asignar el nuevo socket
						new_client.events = POLLIN | POLLOUT; // Esperar eventos de entrada y salida
						fds.push_back(new_client); //Añadirlo al vector de fds (clientes)
					}
					else
					{
						// Limpiar el buffer antes de recibir nuevos datos
						std::fill(buffer.begin(), buffer.end(), '\0');
						std::string req = ft_recv(fds[i].fd);
						if (req.empty())
						{
							handle_error(fds[i].fd, "closed client connection");
							fds.erase(fds.begin() + i); // Eliminar el socket de la lista
							--i; // Decrementar 'i' para no omitir el siguiente cliente
						}
						else
						{
							// bool bodyInProgress = false;
							// size_t n = buffer.find("Content-Length: ");
							// if (n != std::string::npos)
							// {
							// 	size_t end = buffer.find('\n', n);
							// 	std::string ct_len_aux = buffer.substr(n + 16, end - n - 16);
							// 	int ct_len = ft_stoi(ct_len_aux);
							// 	std::string fullRequest = buffer.substr(0);
							// 	if (ct_len > 0)
							// 	{
							// 		if (readUntilContentLength(fds[i].fd, fullRequest, ct_len))
							// 		{
							// 		    std::cout << "Solicitud completa recibida.\n";
							// 		    // request.parseBody(fullRequest);  // Método que parsea el cuerpo del mensaje
							// 		}
							// 		else
							// 		    std::cerr << "Error al leer el cuerpo del mensaje.\n";
							// 	}
							// 	std::cout << std::endl << fullRequest << std::endl;
							// }

							HttpRequest auxRequest(req);
							if (auxRequest.checkServer(conf))
							{
								if (auxRequest.validateLocation()) // Comprobar la ubicación
								{
									auxRequest.parseHttpRequest();  // Procesar la solicitud
								}
							}
							// std::cout << "Procesada solicitud." << std::endl;
							std::string response = auxRequest.generateResponse(); // Generar respuesta
							// std::cout << "Generada respuesta." << std::endl;
							int bytesSent = send(fds[i].fd, response.c_str(), response.size(), 0);
							if (bytesSent == -1)
							{
								std::cerr << "Error al enviar datos: " << strerror(errno) << std::endl;
								handle_error(fds[i].fd, "Error al enviar respuesta");
							}
							// else
							// {
							// 	std::cout << "Respuesta enviada correctamente." << std::endl;
							// }
						}
					}
				}
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}


/*Falta:
  - Nombre del servidor
  - Controlar el client body size.
  - Controlar lo del autoindex. (listar los archivos disponibles en la ruta)
  - Chequear que no haya mismo host y port en varios servidores.
  - Controlar la devolucion de archivos. ¿GET diferente para cada url?
  - 


*/