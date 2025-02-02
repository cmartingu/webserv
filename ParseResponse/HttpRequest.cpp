#include "HttpRequest.hpp"

void	HttpRequest::setHttpReq(const std::string& rawRequest, bool second, int ct_len)
{
	this->rawRequest = rawRequest;
	std::istringstream stream(this->rawRequest);


	// Parsear la línea de solicitud
	std::string requestLine;
	std::getline(stream, requestLine);
	parseRequestLine(requestLine);

	// Parsear los encabezados
	content_length = ct_len;
	parseHeaders(stream, ct_len);

	getHost();

	if (rawRequest.find("multipart/form-data") != std::string::npos && !second)
	{
		this->fragmented = true;
	}
	else
		this->fragmented = false;
	// Inicializar atributos de la respuesta por defecto
	status = "200 OK"; // Estado predeterminado
	responseHeaders["Content-Type"] = "text/html";

}

HttpRequest::HttpRequest()
{
}

void HttpRequest::getHost(void)
{
    if (headers.find("Host") != headers.end()) {
        std::string hostHeader = (headers["Host"]);
        
        // Buscar el puerto en la cabecera (si existe)
        size_t colon_pos = hostHeader.find(':');
        
        if (colon_pos != std::string::npos) {
            // Hay un puerto especificado
            std::string host = hostHeader.substr(0, colon_pos);
            std::stringstream ss(hostHeader.substr(colon_pos + 1));
            ss >> port;
            this->host = host;
        }
        else {
            // No hay puerto, usar puerto por defecto (por ejemplo, 8080)
            host = hostHeader;
            port = 8080;  // Puerto por defecto para HTTP, pero podrías cambiarlo si es necesario
        }
    } else {
        // Si no hay cabecera "Host", usar valores predeterminados
        host = "127.0.0.1"; // O cualquier valor predeterminado
        port = 8080;        // Puerto por defecto
    }
}

void HttpRequest::parseHeaders(std::istream& stream, int ct_len)
{
	std::string headerLine;
	while (std::getline(stream, headerLine) && headerLine != "\r") {
		size_t separator = headerLine.find(':');
		if (separator != std::string::npos) {
			std::string key = headerLine.substr(0, separator);
			std::string value = headerLine.substr(separator + 2);
			if (key == "Content-Length" && ct_len < 0)
			{
				std::istringstream oss(value);
				oss >> content_length;
			}
			//std::cout << "key: " << key << "	Value: " << value << std::endl;
			headers[key] = value;
		}
	}
}

void HttpRequest::parseRequestLine(const std::string& requestLine)
{
	std::istringstream lineStream(requestLine);
	lineStream >> method >> path >> version;

	// Extraer parámetros de consulta (si existen)
	size_t queryPos = path.find('?');
	if (queryPos != std::string::npos) {
		queryString = path.substr(queryPos + 1);
		path = path.substr(0, queryPos);
	}
}

void HttpRequest::setResponse(const std::string& newStatus, const std::string& content, const std::string& contentType)
{
	status = newStatus;
	responseBody = content;
	responseHeaders["Content-Type"] = contentType;
	std::ostringstream oss;
	oss << content.size();
	responseHeaders["Content-Length"] = oss.str();
	// Depuración: muestra los valores configurados
    /*std::cout << "Status: " << status << std::endl;
    std::cout << "Content-Type: " << responseHeaders["Content-Type"] << std::endl;
    std::cout << "Content-Length: " << responseHeaders["Content-Length"] << std::endl;
    std::cout << "Cuerpo de la respuesta: " << content << std::endl;*/
}


std::string HttpRequest::getPathLocation(std::string path) const
{
	size_t end = path.find('/', 1);
	if (end ==  std::string::npos)
		return "/";
	return path.substr(0, end);
}

std::string HttpRequest::generateResponse() const
{
    std::ostringstream responseStream;

    responseStream << version << " " << status << "\r\n";
    // Agregar los encabezados HTTP
    for (std::map<std::string, std::string>::const_iterator it = responseHeaders.begin(); it != responseHeaders.end(); ++it)
	{
		if (it->first == "Content-Type" && getPathLocation(path) == "/cgi-bin" && method == "GET")
			responseStream << it->first << ": " << it->second << "; charset=utf-8" << "\r\n";
		else
			responseStream << it->first << ": " << it->second << "\r\n";
	}
    responseStream << "\r\n";
    responseStream << responseBody;
	//std::string fullResponse = responseStream.str();
	//std::cout << "Respuesta completa generada: " << std::endl;
	//std::cout << fullResponse << std::endl;
    return responseStream.str();
}


bool HttpRequest::isMethodAllowed(const std::vector<std::string>& allowedMethods) const
{
	return std::find(allowedMethods.begin(), \
					allowedMethods.end(), \
					method) != allowedMethods.end();
}

bool HttpRequest::checkServer(Config &conf)
{
	//this->printRequestDetails();
	if (host == "localhost")
	{
        host = "127.0.0.1";
    }
    for (int i = 0; i < conf.getNbServers(); ++i)
	{
        Server* serverConfig = conf.getServer(i); // Obtener el servidor
		in_addr_t serverHost = serverConfig->getHost(); // Obtener la IP del servidor
		std::string server_name = serverConfig->getServerName(); 

        // Convertir la dirección IP del servidor a std::string
        struct in_addr addr;
        addr.s_addr = serverHost;
        char strHost[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr, strHost, INET_ADDRSTRLEN);

        std::string serverHostStr = strHost;
		//std::cout<< "Host: " << strHost << " - reqHost: " << host << " - Name: " << server_name << std::endl;
		//std::cout<< "Port: " << serverConfig->getPort() << " - reqPort: " << port << std::endl;
        if (serverConfig->getPort() == port && (strHost == host || host == server_name)) // Verificar si el puerto coincide con el del servidor
		{
			server = serverConfig;
			return true;
        }
    }
    // Si no se encontró el servidor con el puerto adecuado
    setResponse("404 Not Found", 
            "<html><body><h1>404 Not Found</h1><p>Recurso no encontrado servidor.</p></body></html>", 
            "text/html");
	return false;
}

bool containsPath(std::string location, std::string path)
{
	for (size_t i = 0; i < location.length(); i++)
	{
		if (location[i] != path[i])
			return false;
	}
	return true;
}

std::string getFinalPath(std::string path)
{
	size_t end = path.find('/', 1);
	if (end ==  std::string::npos)
		return path;
	return path.substr(end);
}

bool HttpRequest::properBodySize()
{
	if (headers.count("Content-Length") > 0)
	{
		std::istringstream line(headers["Content-Length"]);
		unsigned long aux;
		line >> aux;
		//std::cout << "Server Body Size:" << server->getClientMaxBodySize() << "Request Content Lenght: " << aux << std::endl;
		if (server->getClientMaxBodySize() <= aux)
			return false;
	}
	return true;
}


bool HttpRequest::validateLocation()
{
	// std::cout << "checking location" << std::endl;
	std::vector<Location>& locations = server->getLocations();
	for (size_t j = 0; j < locations.size(); ++j)
	{
		// std::cout << "PATH: " << path << std::endl << "Location:" << locations[j].getPath() << " - Path:" << getPathLocation(path) <<std::endl;
		// std::cout << getPathLocation(path) << std::endl;
		if (locations[j].getPath() == getPathLocation(path))
		{
			location = &locations[j];
			// std::cout << "Path encontrado." << std::endl;
			// Asumimos que tienes un método getPath() que obtiene el path
			// Si la ubicación existe, comprobar si el método está permitido
			if (!location->getReturn().empty())
			{
				path = location->getReturn() + "/" + getFinalPath(path);
				this->validateLocation();
			}
			if (!isMethodAllowed((*location).getMethods()))
			{
				std::cout << "metodo no permitido" << std::endl;
				setResponse("405 Method Not Allowed", 
						"<html><body><h1>405 Method Not Allowed</h1><p>Metodo no permitido.</p></body></html>", 
						"text/html"); // Si el método no está permitido
				return false;
			}
			if (!properBodySize())
			{
				setResponse("413 Content Too Large", 
						"<html><body><h1>413 Content Too Large</h1><p>Contenido muy largo.</p></body></html>", 
						"text/html");
				return false;
			}
			// Si todo es correcto, puedes realizar otras acciones aquí si es necesario
			return true;
		}
	}
	// Si no se encontró la location en el servidor
	setResponse("404 Not Found", 
	"<html><body><h1>404 Not Found</h1><p>Recurso no encontrado en location.</p></body></html>", 
	"text/html");
	
	return false;
}

void HttpRequest::parseHttpRequest()
{
	// std::cout << "Parsing Request : " << method << std::endl;
	if (method == "GET")
	{
		methodGet();
	}
	else if (method == "POST")
	{
		methodPost();
	}
	else if (method == "DELETE")
	{
		methodDelete();
		//setResponse("405 Method Not Allowed", 
		//"<html><body><h1>Error</h1><p>DELETE no soportado en este servidor estático.</p></body></html", 
		//"text/html");
	}
	else
	{
		setResponse("501 Not Implemented", 
		"<html><body><h1>Error</h1><p>Método no implementado.</p></body></html>", 
		"text/html");
	}
}

bool isDirectory(const std::string& path)
{
	struct stat info;

	// Verificar si la ruta es válida
	if (stat(path.c_str(), &info) != 0) 
		return false;

	// Comprobar si es un directorio
	return (info.st_mode & S_IFDIR) != 0;
}

void HttpRequest::generateAutoIndex(const std::string& directoryPath)
{
    DIR* dir;
    struct dirent* entry;
    struct stat fileStat;
    std::stringstream html;

    // Abrir el directorio
    dir = opendir(directoryPath.c_str());
    if (!dir)
	{
		status = "500 Internal Error";
        responseBody =  "<html><body><h1>Error: No se pudo abrir el directorio.</h1></body></html>";
		responseHeaders["Content-Type"] = "text/html";
		std::ostringstream oss;
		oss << responseBody.size();
		responseHeaders["Content-Length"] = oss.str();
		return ;
    }

    // Construir el encabezado del HTML
    html << "<html><head><title>AutoIndex</title></head><body>";
    html << "<h1>Indice de " << directoryPath << "</h1>";
    html << "<ul>";

    // Iterar sobre los archivos y directorios
    while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue ;
        std::string fullPath = directoryPath + "/" + entry->d_name;

        // Obtener información sobre el archivo
        if (stat(fullPath.c_str(), &fileStat) == 0) {
            if (S_ISDIR(fileStat.st_mode)) {
                // Es un directorio
           		html << "<li><b>[Directorio]</b> <a href=\"" << entry->d_name << "/\">" 
                   	<< entry->d_name << "/</a></li>";
            } else {
                // Es un archivo
                html << "<li><b>[Archivo]</b> <a href=\"" << entry->d_name << "\">" 
                     << entry->d_name << "</a></li>";
            }
        }
    }

    // Cerrar el directorio y finalizar el HTML
    closedir(dir);
    html << "</ul></body></html>";
	status = "200 OK";
	responseHeaders["Content-Type"] = "text/html";
	std::ostringstream oss;
	oss << html.str().size();
	responseHeaders["Content-Length"] = oss.str();
	responseBody = html.str();
}

std::string HttpRequest::executeCGI(const std::string &cgiPath, const std::string &scriptPath)
{
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        throw std::runtime_error("Error al crear el pipe: " + std::string(strerror(errno)));
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        throw std::runtime_error("Error al hacer fork: " + std::string(strerror(errno)));
    }

    if (pid == 0) { // Proceso hijo
        // Redirige la salida estándar hacia el pipe
        close(pipefd[0]); // Cierra el extremo de lectura
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            close(pipefd[1]);
            exit(EXIT_FAILURE);
        }
        close(pipefd[1]);

        // Ejecuta el script CGI
        char *argv[] = {const_cast<char*>(cgiPath.c_str()), const_cast<char*>(scriptPath.c_str()), NULL};
        execve(cgiPath.c_str(), argv, NULL);

        // Si execve falla
        exit(EXIT_FAILURE);
    } else { // Proceso padre
        close(pipefd[1]); // Cierra el extremo de escritura

        // Lee la salida del pipe
        std::string output;
        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, bytesRead);
        }
        close(pipefd[0]);

        // Espera al hijo
        int status;
        waitpid(pid, &status, 0);

        // Comprueba si hubo un error en el proceso hijo
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            this->status = "500 Internal Server Error";
		else
			this->status = "200 OK";
        return output;
    }
}




void HttpRequest::methodGet()
{
	// std::string filePath = this->location->getRootLocation() + getFinalPath(path);
	std::string filePath = this->location->getRootLocation() + path;
	if (path.find("..") != std::string::npos)
	{
		std::cerr << "Intento de acceso no autorizado: " << path << std::endl;
		setResponse("403 Forbidden", 
			"<html><body><h1>403 Forbidden</h1><p>Acceso denegado.</p></body></html>", 
			"text/html");
		return;
	}

	if (isDirectory(filePath))
	{
		if (!this->location->getIndexLocation().empty()) //añadir el index
		{
			filePath += this->location->getIndexLocation();
		}
		else if (this->location->getAutoindex()) //hacer autoindex
		{
			generateAutoIndex(filePath);
			return ;
		}
		else
		{
			//estas pidiendo una carpeta y no tiene index ni autoindex
		}
	}

	if (getPathLocation(path) == "/cgi-bin")
	{
		std::map<std::string, std::string>::const_iterator it = (this->location->getCgiMix()).find(".py");
		if (it == this->location->getCgiMix().end())
			it = (this->location->getCgiMix()).find(".sh");
		if (it == this->location->getCgiMix().end())
			throw Server::ServerErrorException("CGI error");
		std::string cgi_path = it->second;
		std::string output = executeCGI(cgi_path, filePath);
		setResponse(status, output, "text/html");
		return ;
	}

	// Intenta abrir el archivo
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) // Si el archivo no existe, devolver mensaje de error 404
	{
		std::cout << "Archivo no encontrado" << std::endl;
		file.open(this->server->getPathErrorPage(404).c_str());
		if (!file.is_open())
		{
			std::cerr << "Error crítico: No se pudo abrir 404.html tampoco." << std::endl;
			setResponse("500 Internal Server Error", 
				"<html><body><h1>500 Internal Server Error</h1><p>No se pudo procesar la solicitud.</p></body></html>", 
				"text/html");
			return;
		}
		status = "404 Not Found";
	}
	else
	{
		status = "200 OK";
	}
	// Lee el contenido del archivo en un string
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	setResponse(status, buffer.str(), "text/html");
}

size_t	ft_auxFind(std::string principal, std::string sec)
{
	if (sec.empty() || principal.length() < sec.length()) {
        return std::string::npos; // Si `sec` está vacío o es más largo que `principal`
    }

    for (size_t i = 0; i <= principal.length() - sec.length(); i++) {
        size_t j = 0;
        while (j < sec.length() && principal[i + j] == sec[j]) {
            j++;
        }
        if (j == sec.length()) { // Se encontró `sec` completo
            return i;
        }
    }
    return std::string::npos;
}

std::string HttpRequest::getFileName()
{
	//std::cout << "Getting Filename: " << rawRequest << std::endl;
	size_t dispositionStart = rawRequest.find("Content-Disposition", 0);
	if (dispositionStart == std::string::npos)
	{
		setResponse("400 Bad Request", 
            "<html><body><h1>Error</h1><p>No se encontró Content-Disposition.</p></body></html>", 
            "text/html");
		return "error";
	}

	size_t filenameStart = rawRequest.find("filename=\"", dispositionStart);
	if (filenameStart == std::string::npos)
	{
		setResponse("400 Bad Request", 
            "<html><body><h1>Error</h1><p>No se encontró nombre del archivo.</p></body></html>", 
            "text/html");
		return "error";
	}
	filenameStart += 10; // Saltar 'filename="'
	size_t filenameEnd = rawRequest.find("\"", filenameStart);
	if (filenameEnd == std::string::npos)
	{
		setResponse("400 Bad Request", 
            "<html><body><h1>Error</h1><p>Nombre del archivo mal formado.</p></body></html>", 
            "text/html");
		return "error";
	}
	std::string filename = rawRequest.substr(filenameStart, filenameEnd - filenameStart);

	return filename;
}

void HttpRequest::methodPost()
{
	// std::cout << rawRequest << std::endl;
	//this->printRequestDetails();
	std::string boundary = headers["Content-Type"];
	size_t boundaryPos = boundary.find("boundary=");
	if (boundaryPos == std::string::npos)
	{
		setResponse("400 Bad Request", 
            "<html><body><h1>Error</h1><p>Formato inválido.</p></body></html>", 
            "text/html");
		return;
	}

	boundary = "--" + boundary.substr(boundaryPos + 9);
	if (!boundary.empty() && boundary[boundary.size() - 1] == '\r')
		boundary.erase(boundary.size() - 1);

	size_t fileStart = rawRequest.find(boundary) + boundary.length() + 1;
	fileStart = rawRequest.find("\r\n\r\n", rawRequest.find("Content-Type", fileStart)) + 4;

	std::string auxfileContent = rawRequest.substr(fileStart);
	size_t fileEnd = auxfileContent.find("\r\n" + boundary);
	std::cout << "Start: " << fileStart << " - End: " << fileEnd << std::endl;
	if (fileStart == std::string::npos || fileEnd == std::string::npos)
	{
		setResponse("404 Not Found", 
            "<html><body><h1>Errorsito</h1><p>No se encontró archivo.</p></body></html>", 
            "text/html");
		return;
	}

	std::string filename = this->getFileName();
	if (filename == "error")
		return;

	// Extraer el contenido del archivo
	std::string fileContent = auxfileContent.substr(0, fileEnd);
	// std::cout << "CONTENIDO\n" << fileContent << "\nFIN CONTENIDO" << std::endl;
	// std::cout << fileContent << std::endl;
	//char cwd[PATH_MAX];
	//std::cout << "Estoy en: " << getcwd(cwd, sizeof(cwd)) << std::endl;
	std::string uploadPath = "./uploads/" + filename;
	std::ofstream outFile(uploadPath.c_str(), std::ios::binary);
	if (!outFile.is_open())
	{
		setResponse("500 Internal Server Error", 
            "<html><body><h1>Error</h1><p>No se pudo guardar el archivo.</p></body></html>", 
            "text/html");
        return;
	}
	outFile.write(fileContent.c_str(), fileContent.length());
	outFile.close();

	setResponse("201 Created", 
        "<html><body><h1>Archivo subido con exito</h1></body></html>", 
        "text/html");
}


void HttpRequest::methodDelete()
{
	const std::string directory = "./uploads";
    DIR* dir = opendir(directory.c_str());
    
    // Verificación de que el directorio se ha abierto correctamente
    if (dir == NULL) {
        setResponse("404 Not Found", 
                    "<html><body><h1>Error</h1><p>El directorio no existe.</p></body></html>", 
                    "text/html");
        return;
    }

    struct dirent* entry;
    // Lectura del contenido del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar los directorios "." y ".."
        if (entry->d_name[0] == '.') continue;

        std::string filePath = directory + "/" + entry->d_name;
        // Eliminar el archivo
        if (remove(filePath.c_str()) != 0) {
            setResponse("500 Internal Server Error", 
                        "<html><body><h1>Error</h1><p>No se pudo eliminar los archivos.</p></body></html>", 
                        "text/html");
            closedir(dir);
            return;
        }
    }

    // Cerrar el directorio una vez terminado el proceso
    closedir(dir);

    setResponse("200 OK", 
                "<html><body><h1>Archivos eliminados con éxito</h1></body></html>", 
                "text/html");
}

void HttpRequest::printRequestDetails() const
{
    std::cout << "---- HTTP Request Details ----" << std::endl;

    // Método, ruta y versión
    std::cout << "Method: " << method << std::endl;
    std::cout << "Path: " << path << std::endl;
    std::cout << "Version: " << version << std::endl;

    // Query string
    if (!queryString.empty()) {
        std::cout << "Query String: " << queryString << std::endl;
    } else {
        std::cout << "Query String: None" << std::endl;
    }

    // Headers
    std::cout << "Headers:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }

    // Cuerpo de la solicitud
    if (!body.empty()) {
        std::cout << "Body:" << std::endl;
        std::cout << body << std::endl;
    } else {
        std::cout << "Body: None" << std::endl;
    }

    std::cout << "--------------------------------" << std::endl;
}


int HttpRequest::getContentLength()
{
	std::string contentLength = this->headers["Content-length"];
	int result = 0;
	std::stringstream(contentLength) >> result;
	return result;
}


