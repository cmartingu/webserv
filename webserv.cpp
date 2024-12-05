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
		}

		std::vector<Client> clients;
		while (true)
		{
			int ret = poll(fds.data(), fds.size(), -1); // Espera indefinidamente por eventos en los servidores
			if (ret == -1)
			{
				std::cerr << "Error en poll: " << strerror(errno) << std::endl;
				break; //Salir del bucle si hay error
			}

			for (size_t i = 0; i < fds.size(); ++i) //Empieza en 1 porque el 0 es el servidor
			{
				if (fds[i].revents & POLLIN)
				{
					// std::cout << i << std::endl;
					// std::cout << "----- Entrada de algo -------" << std::endl;
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
						// std::cout << "Nueva conexión aceptada en el socket " << new_socket << std::endl;
						if (setNonBlocking(new_socket))
							continue; //Nueva iteracion del bucle
						pollfd new_client;
						new_client.fd = new_socket; // Asignar el nuevo socket
						new_client.events = POLLIN | POLLOUT; // Esperar eventos de entrada y salida
						fds.push_back(new_client); //Añadirlo al vector de fds (clientes)
						Client clientClass(client_addr, new_client);
						clients.push_back(clientClass);
					}
					else
					{
						// Limpiar el buffer antes de recibir nuevos datos
						std::fill(buffer.begin(), buffer.end(), '\0');

						int bytesRead = recv(fds[i].fd, &buffer[0], buffer.size(), 0);
						if (bytesRead <= 0)
						{
							std::ostringstream oss;
							oss << i;
							std::string nb = oss.str();
							handle_error(fds[i].fd, "closed client connection " + nb);
							fds.erase(fds.begin() + i); // Eliminar el socket de la lista
							--i; // Decrementar 'i' para no omitir el siguiente cliente
						}
						else
						{
							// std::cout << "\ncontent:\n" << buffer  << "\nEND OF CONTENT" << std::endl;
							clients[i - conf.getNbServers()].addRequest(buffer);
							if (!(clients[i - conf.getNbServers()].clientRequests[(clients[i - conf.getNbServers()]).nb_req - 1].fragmented))
							{
								if (clients[i - conf.getNbServers()].clientRequests[(clients[i - conf.getNbServers()]).nb_req - 1].checkServer(conf))
								{
									if (clients[i - conf.getNbServers()].clientRequests[(clients[i - conf.getNbServers()]).nb_req - 1].validateLocation())
										clients[i - conf.getNbServers()].clientRequests[(clients[i - conf.getNbServers()]).nb_req - 1].parseHttpRequest();
								}
								std::string response = clients[i - conf.getNbServers()].clientRequests[(clients[i - conf.getNbServers()]).nb_req - 1].generateResponse();
								int bytesSent = send(fds[i].fd, response.c_str(), response.size(), 0);
								if (bytesSent == -1)
								{
									std::cerr << "Error al enviar datos: " << strerror(errno) << std::endl;
									handle_error(fds[i].fd, "Error al enviar respuesta");
								}
								else
									std::cout << "Respuesta enviada correctamente." << std::endl;
							}
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
