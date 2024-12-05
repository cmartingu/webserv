#include "Server.hpp"

Server::Server(void)
{
    this->port = 0;
	this->host = 0;
	this->serv_name = "";
	this->root = "";
	this->client_max_body_size = MAX_REQUEST_LENGTH;
	this->index = "";
	this->listen_fd = 0;
	this->autoindex = false;
	this->initErrorWebs();
}

Server::Server(Server const &copy)
{
    this->port = copy.port;
	this->host = copy.host;
	this->serv_name = copy.serv_name;
	this->root = copy.root;
	this->client_max_body_size = copy.client_max_body_size;
	this->index = copy.index;
	this->listen_fd = copy.listen_fd;
	this->autoindex = copy.autoindex;
    this->errorWebs = copy.errorWebs;
    this->locations = copy.locations;
    this->server_address = copy.server_address;
}

Server::~Server()
{
}

/****************************PUBLIC METHODS*****************************/

void	Server::initErrorWebs()
{
	this->errorWebs.insert(std::make_pair(301, "")); //moved permanently
	this->errorWebs.insert(std::make_pair(302, "")); //Found(moved temporarily)
	this->errorWebs.insert(std::make_pair(303, "")); //See other
	this->errorWebs.insert(std::make_pair(307, "")); //Temporary Redirect
	this->errorWebs.insert(std::make_pair(308, "")); //Permanent redirect
	this->errorWebs.insert(std::make_pair(400, "")); //Bad request
	this->errorWebs.insert(std::make_pair(401, "")); //Unauthorized
	this->errorWebs.insert(std::make_pair(402, "")); //Payment required
	this->errorWebs.insert(std::make_pair(403, "")); //Forbidden
	this->errorWebs.insert(std::make_pair(404, "")); //Not found
	this->errorWebs.insert(std::make_pair(405, "")); //Method not allowed
	this->errorWebs.insert(std::make_pair(406, "")); //Not acceptable
	this->errorWebs.insert(std::make_pair(408, "")); //Request timeout
	//this->errorWebs.insert(std::make_pair(413, "<html><body><h1>413 Content Too Large</h1><p>Contenido muy largo.</p></body></html>text/html"));
	this->errorWebs.insert(std::make_pair(429, "")); //Too Many Request
	this->errorWebs.insert(std::make_pair(500, "")); //Internal server error
	this->errorWebs.insert(std::make_pair(501, "")); //Not implemented
	this->errorWebs.insert(std::make_pair(502, "")); //Bad Gateway
	this->errorWebs.insert(std::make_pair(503, "")); //Service unaviable
	this->errorWebs.insert(std::make_pair(504, "")); //Gateway timeout
	this->errorWebs.insert(std::make_pair(505, "")); //HTTP version not supported
}

int		Server::typeOfPath(std::string const path)
{
	struct stat	buffer;
	int			result;
	
	result = stat(path.c_str(), &buffer);
	if (result == 0)
	{
		if (buffer.st_mode & S_IFREG) //archivo
			return 1;
		else if (buffer.st_mode & S_IFDIR) //directorio
			return 2;
		else
			return 3;
	}
	else
		return -1;
}

static int	checkFile(std::string const path, int mode)
{
	return (access(path.c_str(), mode));
}

static int	isFileExistAndReadable(std::string const path, std::string const index)
{
	if (Server::typeOfPath(index) == 1 && checkFile(index, 4) == 0)
		return (0);
	if (Server::typeOfPath(path + index) == 1 && checkFile(path + index, 4) == 0)
		return (0);
	return (-1);
}

void	Server::tryLocation(Location &location)//comrpobar si está bien configurada location
{
	if (location.getPath() == "/cgi-bin") //comprobar aparte
	{
		if (location.getCgiExtension().empty() || location.getCgiPath().empty())
			throw Config::ConfFileException("Wrong \"/cgi-bin\" location configuration, must have cgi_path and cgi_extension");

		if (access(location.getIndexLocation().c_str(), 4) < 0)
		{
			std::string auxIndex = location.getRootLocation() + location.getPath() + "/" + location.getIndexLocation();
			if (typeOfPath(auxIndex) != 1)
			{
				std::string rootPath = getcwd(NULL, 0);
				location.setRootLocation(rootPath);
				auxIndex = rootPath + location.getPath() + "/" + location.getIndexLocation();
			}
			if (auxIndex.empty() || typeOfPath(auxIndex) != 1 || access(auxIndex.c_str(), 4) < 0)
				throw Config::ConfFileException("Wrong \"/cgi-bin\" location configuration");
		}
		if (location.getCgiPath().size() != location.getCgiExtension().size())
			throw Config::ConfFileException("Wrong \"/cgi-bin\" location configuration");

		std::vector<std::string>::const_iterator it;
		for (it = location.getCgiPath().begin(); it != location.getCgiPath().end(); ++it)
		{
			if (typeOfPath(*it) < 0)
				throw Config::ConfFileException("Wrong \"/cgi-bin\" location configuration");
		}
		std::vector<std::string>::const_iterator itPath;
		for (it = location.getCgiExtension().begin(); it != location.getCgiExtension().end(); ++it)
		{
			if (*it != ".py" && *it != ".sh")
				throw Config::ConfFileException("Wrong \"/cgi-bin\" location configuration");
			for (itPath = location.getCgiPath().begin(); itPath != location.getCgiPath().end(); ++itPath)
			{
				if (*it == ".py")
				{
					if ((*itPath).find("python") != std::string::npos)
						location.addCgiMix(*it, *itPath);
				}
				else if (*it == ".sh")
				{
					if ((*itPath).find("bash") != std::string::npos)
						location.addCgiMix(*it, *itPath);
				}
			}
		}
		if (location.getCgiPath().size() != location.getCgiExtension().size())
			throw Config::ConfFileException("Wrong \"/cgi-bin\" location configuration");
	}
	else
	{
		if (location.getPath()[0] != '/')
			throw Config::ConfFileException("Error validating location path \"" + location.getPath() + "\"");
		if (location.getRootLocation().empty())
			location.setRootLocation(this->root);
		if (isFileExistAndReadable(location.getRootLocation() + location.getPath() + "/", location.getIndexLocation()))
			return ;
		if (!location.getReturn().empty())//hay return
		{
			if (isFileExistAndReadable(location.getRootLocation(), location.getReturn()))
				throw Config::ConfFileException("Error validating return \"" + location.getReturn() + "\"");
		}
		if (!location.getAlias().empty())//hay alias
		{
			if (isFileExistAndReadable(location.getRootLocation(), location.getAlias()))
				throw Config::ConfFileException("Error validating alias \"" + location.getAlias() + "\"");
		}
	}
}

bool	Server::isValidHost(std::string hostName) const //check if host is valid
{
	struct sockaddr_in sockaddr;
	if (inet_pton(AF_INET, hostName.c_str(), &(sockaddr.sin_addr)))
		return true;
	return false;
}

void	Server::endWithSemicolon(std::string &token)//comprueba que acaba en ; y lo elimina
{
	size_t	pos = token.find(';');
	if (pos == std::string::npos)
		throw Config::ConfFileException("Token \"" + token + "\" must end with ;");

	for (size_t i = pos + 1; i < token.size(); i++)
	{
		if (!isspace(token[i]))
			throw Config::ConfFileException("Wrong token \"" + token + "\"");
	}
	token.erase(pos);
}

void	Server::setDefaultErrorWebs()
{
	std::string aux404 = getcwd(NULL, 0);

	if (this->errorWebs[403].empty())
	{
		if (access((aux404 + "/configs/defaultConfig/403.html").c_str(), 0) == -1 || access((aux404 + "/configs/defaultConfig/403.html").c_str(), 4) == -1) //comprueba que el archivo exista y sea legible
			throw Config::ConfFileException("Could not access default error page");
		this->errorWebs[403] = (aux404 + "/configs/defaultConfig/403.html").c_str();
	}
	if (this->errorWebs[404].empty())
	{
		if (access((aux404 + "/configs/defaultConfig/404.html").c_str(), 0) == -1 || access((aux404 + "/configs/defaultConfig/404.html").c_str(), 4) == -1) //comprueba que el archivo exista y sea legible
			throw Config::ConfFileException("Could not access default error page");
		this->errorWebs[404] = (aux404 + "/configs/defaultConfig/404.html").c_str();
	}
	if (this->errorWebs[500].empty())
	{
		if (access((aux404 + "/configs/defaultConfig/500.html").c_str(), 0) == -1 || access((aux404 + "/configs/defaultConfig/500.html").c_str(), 4) == -1) //comprueba que el archivo exista y sea legible
			throw Config::ConfFileException("Could not access default error page");
		this->errorWebs[500] = (aux404 + "/configs/defaultConfig/403.html").c_str();
	}
}

/****************************SETTERS*****************************/

void	Server::setServerName(std::string server_name)
{
	if (!this->serv_name.empty()) //lo q hace nginx
		return ;
	endWithSemicolon(server_name);
	this->serv_name = server_name;
}

void	Server::setHost(std::string host)
{
	endWithSemicolon(host);
	if (host == "localhost")
		host = "127.0.0.1";
	if (!isValidHost(host))
		throw Config::ConfFileException("Invalid host \"" + host + "\"");
	this->host = inet_addr(host.data());
}

void	Server::setRoot(std::string root)
{
	endWithSemicolon(root);

	if (typeOfPath(root) == 2)
		this->root = root;
	else
	{
		char dir[1024];
		getcwd(dir, 1024);
		std::string finalRoot = dir + root;
		if (typeOfPath(finalRoot) != 2)
			throw Config::ConfFileException("Invalid root \"" + root + "\"");
		this->root = finalRoot;
	}
}

void	Server::setFd(int &fd)
{
	this->listen_fd = fd;
}

void	Server::setPort(std::string port)
{
	endWithSemicolon(port);

	for (size_t i=0; i < port.length(); i++)
	{
		if (!isdigit(port[i]))
			throw Config::ConfFileException("Invalid port: \"" + port + "\"");
	}
	int	finalPort = atoi(port.c_str());
	if (finalPort < 1 || finalPort > 65636)
		throw Config::ConfFileException("Invalid port: \"" + port + "\"");
	this->port = (uint16_t)finalPort;
}

void	Server::setClientMaxBodySize(std::string maxBodySize)
{
	endWithSemicolon(maxBodySize);

	for (size_t i=0; i < maxBodySize.length(); i++)
	{
		if (!isdigit(maxBodySize[i]))
			throw Config::ConfFileException("Invalid client_max_body_size: \"" + maxBodySize + "\"");
	}
	long finalSize = atoi(maxBodySize.c_str());
	if (finalSize < 1)
		throw Config::ConfFileException("Invalid client_max_body_size: \"" + maxBodySize + "\"");
	this->client_max_body_size = (unsigned long)finalSize;
}

void	Server::setErrorPages(std::vector<std::string> error_webs)
{
	if (error_webs.empty())
		return ;

	for (size_t i = 0; i < error_webs.size(); i++)
	{
		//primero el código
		for (size_t j = 0; j < error_webs[i].length(); j++)
		{
			if (!isdigit(error_webs[i][j]))
				throw Config::ConfFileException("Invalid error_page code: \"" + error_webs[i] + "\"");
		}
		if (error_webs[i].length() != 3)
			throw Config::ConfFileException("Invalid error_page code: \"" + error_webs[i] + "\"");
		int	codeError = atoi(error_webs[i].c_str());
		//ahora la dirección
		i++;
		if (i >= error_webs.size())
			throw Config::ConfFileException("Invalid error_page in file");

		std::string path = error_webs[i];
		endWithSemicolon(path);
		if (typeOfPath(path) == 2)
			throw Config::ConfFileException("Invalid error_page path: \"" + path + "\"");
		if ((this->root).empty())
		{
			std::string auxRoot = getcwd(NULL, 0);
			this->setRoot((auxRoot + ";").c_str());
		}
		if (typeOfPath(this->root + path) != 1)
			throw Config::ConfFileException("Invalid error_page path: \"" + path + "\"");
		if (access((this->root + path).c_str(), 0) == -1 || access((this->root + path).c_str(), 4) == -1) //comprueba que el archivo exista y sea legible
			throw Config::ConfFileException("Invalid error_page path: \"" + path + "\"");

		std::map<unsigned short, std::string>::iterator it = this->errorWebs.find(codeError);
		if (it != errorWebs.end())
			this->errorWebs[codeError] = this->root + path;
		else
			this->errorWebs.insert(std::make_pair(codeError, this->root + path));
	}
}

void	Server::setIndex(std::string &index)
{
	endWithSemicolon(index);
	this->index = index;
}

void	Server::setAutoindex(std::string &autoindex)
{
	endWithSemicolon(autoindex);
	if (autoindex == "on")
		this->autoindex = true;
	else if (autoindex == "off")
		this->autoindex = false;
	else
		throw Config::ConfFileException("Invalid autoindex: \"" + autoindex + "\"");
}

bool	Server::existParam(std::string &str, size_t pos)
{
	for (size_t i = pos; i < str.length(); i++)
	{
		if (!isspace(str[i]))
			return true;
	}
	return false;
}

std::string	Server::paramReturn(std::string &src, int pos)
{
	if (!isspace(src[pos]))
		throw Config::ConfFileException("Wrong syntax in location: \"" + src + "\"");
	for (size_t i = pos; i < src.length(); i++)
	{
		if (!isspace(src[i]))
			return (src.substr(i));
	}
	throw Config::ConfFileException("Wrong syntax in location: \"" + src + "\"");
}

std::vector<std::string> Server::splitSpace(std::string &methods)
{
	std::vector<std::string>	finalVector;
	size_t						start = 0;
	size_t						end;

	while ((end = methods.find(' ', start)) != std::string::npos)
	{
		if (end != start)
			finalVector.push_back(methods.substr(start, end - start));
		start = end + 1;
	}

	if (start < methods.size())
		finalVector.push_back(methods.substr(start));
	return finalVector;
}

void	Server::parseExtensions(std::vector<std::string> &extensions, std::string &auxExt)
{
	for (size_t i = 0; i < auxExt.length(); i++)
	{
		if (!isspace(auxExt[i]))//guardar en extensions la extension
		{
			int j;
			for (j = 0; auxExt[i] && !isspace(auxExt[i]); j++)
				i++;
			extensions.push_back(auxExt.substr(i - j, j));
		}
	}
}

void	Server::parseCgiPath(std::vector<std::string> &paths, std::string &auxPaths)
{
	for (size_t i = 0; i < auxPaths.length(); i++)
	{
		if (!isspace(auxPaths[i]))
		{
			int j;
			for (j = 0; auxPaths[i] && !isspace(auxPaths[i]); j++)
				i++;
			if (auxPaths.substr(i - j, j).find("/python") == std::string::npos && auxPaths.substr(i - j, j).find("/bash") == std::string::npos)
				throw Config::ConfFileException("Invalid cgi_path in location: \"" + auxPaths.substr(i - j, j) + "\"");
			paths.push_back(auxPaths.substr(i - j, j));
		}
	}
}

void	Server::setLocation(std::string &name, std::vector<std::string> &args)
{
	Location	location;
	bool		saved_methods = false;
	bool		maxBodySize = false;

	location.setPath(name);
	for (size_t i = 0; i < args.size(); i++) //cada argumento es una linea de location
	{
		if (args[i].substr(0, 4) == "root" && existParam(args[i], 4))
		{
			if (!location.getRootLocation().empty())
				throw Config::ConfFileException("Duplicated root in location: \"" + args[i] + "\"");

			std::string auxRoot = paramReturn(args[i], 4);
			endWithSemicolon(auxRoot);
			if (typeOfPath(auxRoot) == 2)
				location.setRootLocation(auxRoot);
			else
				location.setRootLocation(this->root + auxRoot);
		}
		else if (args[i].substr(0, 13) == "allow_methods" && existParam(args[i], 13))
		{
			if (saved_methods)
				throw Config::ConfFileException("Duplicated allow_methods in location");
			std::string auxMethods = paramReturn(args[i], 13);
			endWithSemicolon(auxMethods);
			std::vector<std::string> methods = splitSpace(auxMethods);
			// std::cout << auxMethods << "." << std::endl;
			location.setMethods(methods);
			saved_methods = true;
		}
		else if (args[i].substr(0, 9) == "autoindex" && existParam(args[i], 9))
		{
			std::string auxAutoindex = paramReturn(args[i], 9);
			endWithSemicolon(auxAutoindex);
			location.setAutoindex(auxAutoindex);
		}
		else if (args[i].substr(0, 5) == "index" && existParam(args[i], 5))
		{
			std::string auxIndex = paramReturn(args[i], 5);
			endWithSemicolon(auxIndex);
			location.setIndexPath(auxIndex);
		}
		else if (args[i].substr(0, 6) == "return" && existParam(args[i], 6))
		{
			std::string auxReturn = paramReturn(args[i], 6);
			endWithSemicolon(auxReturn);
			if (auxReturn == "/cgi-bin")
				throw Config::ConfFileException("Parameter return not allowed for cgi");
			if (!location.getReturn().empty())
				throw Config::ConfFileException("Duplicated return in location");
			location.setReturn(auxReturn);
		}
		else if (args[i].substr(0, 5) == "alias" && existParam(args[i], 5))
		{
			std::string auxAlias = paramReturn(args[i], 5);
			endWithSemicolon(auxAlias);
			if (auxAlias == "/cgi-bin")
				throw Config::ConfFileException("Parameter alias not allowed for cgi");
			if (!location.getAlias().empty())
				throw Config::ConfFileException("Duplicated alias in location");
			location.setAlias(auxAlias);
		}
		else if (args[i].substr(0, 7) == "cgi_ext" && existParam(args[i], 7))
		{
			std::string auxExt = paramReturn(args[i], 7);
			endWithSemicolon(auxExt);
			std::vector<std::string> extensions;
			parseExtensions(extensions, auxExt);
			location.setCgiExtension(extensions);
		}
		else if (args[i].substr(0, 8) == "cgi_path" && existParam(args[i], 8))
		{
			std::string auxPath = paramReturn(args[i], 8);
			endWithSemicolon(auxPath);
			std::vector<std::string> paths;
			parseCgiPath(paths, auxPath);
			location.setCgiPath(paths);
		}
		else if (args[i].substr(0, 20) == "client_max_body_size" && existParam(args[i], 20))
		{
			if (maxBodySize)
				throw Config::ConfFileException("Duplicated client_max_body_size in location");
			std::string auxBodySize = paramReturn(args[i], 20);
			endWithSemicolon(auxBodySize);
			for (size_t j = 0; j < auxBodySize.length(); j++)
			{
				if (!isdigit(auxBodySize[j]))
					throw Config::ConfFileException("Invalid client_max_body_size in location: \"" + auxBodySize + "\"");
			}
			long size = atol(auxBodySize.c_str());
			if (size < 1)
				throw Config::ConfFileException("Invalid client_max_body_size in location: \"" + auxBodySize + "\"");
			location.setMaxBodySize(size);
			maxBodySize = true;
		}
		else
			throw Config::ConfFileException("Wrong parámeter in location: \"" + args[i] + "\"");
	}
	// if (location.getPath() != "/cgi-bin" && location.getIndexLocation().empty())
	// 	location.setIndexPath(this->index);
	if (!maxBodySize)
		location.setMaxBodySize(this->client_max_body_size);


	tryLocation(location);
	this->locations.push_back(location);


	// std::string aux = this->locations[0].getCgiKey(".py");
	// std::cout << aux << std::endl;
}

/****************************GETTERS*****************************/

const std::string &Server::getServerName()
{
	return this->serv_name;
}

const uint16_t &Server::getPort()
{
	return this->port;
}

const int &Server::getListenFd()
{
	return this->listen_fd;
}

const in_addr_t &Server::getHost()
{
	return this->host;
}

const unsigned long &Server::getClientMaxBodySize()
{
	return this->client_max_body_size;
}

std::vector<Location> &Server::getLocations()
{
	return this->locations;
}

const std::string &Server::getRoot()
{
	return this->root;
}

const std::map<unsigned short, std::string> &Server::getErrorPages()
{
	return this->errorWebs;
}

const std::string &Server::getIndex()
{
	return this->index;
}

const bool &Server::getAutoindex()
{
	return this->autoindex;
}

const std::string &Server::getPathErrorPage(unsigned int key)
{
	std::map<unsigned short, std::string>::const_iterator it = this->errorWebs.find(key);
	if (it == this->errorWebs.end())
	{
		std::stringstream ss;
		ss << key;
		throw Config::ConfFileException("Could not find error page with key: \"" + ss.str() + "\"");
	}
	return this->errorWebs[key];
}

const std::vector<Location>::iterator Server::getLocationKey(std::string key)
{
	std::vector<Location>::iterator it;
	for (it = this->locations.begin(); it != this->locations.end(); it++)
	{
		if (it->getPath() == key)
			return it;
	}
	throw Config::ConfFileException("Could not find location \"" + key + "\"");
}

bool	Server::emptyErrorWebs()
{
	std::map<unsigned short, std::string>::const_iterator it;
	for (it = this->errorWebs.begin(); it != this->errorWebs.end(); it++)
	{
		if (!(it->second).empty())
			return false;
	}
	return true;
}

void	Server::startServer(void)
{
	if ((this->listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) //protocolo Ipv4, socket TCP
	{
		std::cerr << "Server error: Socket creation error" << std::endl;
		exit(EXIT_FAILURE);
	}

	int	opt = 1;
	setsockopt(this->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	memset(&this->server_address, 0, sizeof(this->server_address));
	this->server_address.sin_family = AF_INET; //protocolo IPv4
	this->server_address.sin_addr.s_addr = this->host;
	this->server_address.sin_port = htons(this->port);

	if (bind(this->listen_fd, (struct sockaddr *) &this->server_address, sizeof(this->server_address)) == -1)
	{
		std::cerr << "Server error: Socket: Bind error" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int Server::createServerSocket()
{
	// Crear socket del servidor
	int server_fd = socket(PF_INET, SOCK_STREAM, 0); //PF_Inet es para IPv4, 
	if (server_fd == -1)
	{
		std::cerr << "Error en '" << "socket" << "': " << strerror(errno) << std::endl;
		close(server_fd);
		return -1;
	}
		

	// Configurar socket del servidor para poder ser reusado en on/off rapido
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) 
	{
		std::cerr << "Error en '" << "set socket" << "': " << strerror(errno) << std::endl;
		close(server_fd);
		return -1;
	}

	server_address.sin_family = AF_INET; //Ipv4
	server_address.sin_addr.s_addr = host; //Acepta conexiones de cualquier IP
	server_address.sin_port = htons(port); //Puerto en formato red.

	if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
	{
		std::cerr << "Error en '" << "bind" << "': " << strerror(errno) << std::endl;
		close(server_fd);
		return -1;
	}

	if (listen(server_fd, 3) == -1) //3 Conex max en cola. SOMAXCONN if wanted all from oper.system
	{
		std::cerr << "Error en '" << "socket" << "': " << strerror(errno) << std::endl;
		close(server_fd);
		return -1;
	}
	
	this->_pollfd.fd = server_fd;
	this->_pollfd.events = POLLIN;
	std::cout << "Servidor escuchando en el puerto " << port << "...\n";
	return server_fd;
}

const pollfd &Server::getPollfd()
{
	return this->_pollfd;
}
