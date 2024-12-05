#include "Config.hpp"

//constructor inicial
Config::Config(std::string const &configPath): nb_servers(0)
{
	std::string content = getConfFile(configPath);
	if (content.empty())
		throw Config::ConfFileException("Empty file");

	removeComments(content);
	saveConfigs(content);
	if (this->servers_config.size() == 0 || this->nb_servers == 0)
		throw ConfFileException("No servers in file [" + configPath + "]");
	// FD_ZERO(&_fd_set);
}

//constructor de copia
Config::Config(Config const &copy)
{
	this->nb_servers = copy.nb_servers;
	for (std::vector<std::string>::const_iterator it = copy.servers_config.begin(); it != copy.servers_config.end(); ++it)
		this->servers_config.push_back(*it);
}

// devuelve el contenido de defaultPath en string
std::string	Config::getConfFile(std::string const &defaultPath)
{
	if (defaultPath.empty() || defaultPath.length() == 0)
		throw Config::ConfFileException("Not existing file");
	
	std::ifstream configfd(defaultPath.c_str());
	if (!configfd || !configfd.is_open())
		throw Config::ConfFileException("Not existing file");

	std::string content, line;
	while (std::getline(configfd, line))
		content += line + '\n';
	return content;
}

//elimina los comentarios del archivo de configuracion
void	Config::removeComments(std::string &content)
{
	size_t	pos = content.find('#');
	while (pos != std::string::npos)
	{
		size_t	final_pos = content.find('\n', pos);
		content.erase(pos, final_pos - pos);
		pos = content.find('#');
	}
}

//Guarda todas las configuraciones de servidores del archivo de configuración pasado como argumento
void	Config::saveConfigs(std::string &configContent)
{
	size_t  start;

	for(start = 0; configContent[start]; start++)//quita espacios del principio
	{
		if (!isspace(configContent[start]))
		{
			configContent = configContent.substr(start);
			break ;
		}
	}
	if (configContent.find("server", 0) == std::string::npos)
		throw Config::ConfFileException("Did not find \"server\" in configuration file");
	start = 0;
	size_t  end = 1;
	while (start != end && start < configContent.length())
	{
		start = findStartPos(start, configContent);
		if (start == std::string::npos)
			break ;
		end = findEndPos(start, configContent);
		if (end == std::string::npos)
			throw ConfFileException("Problem with scope{}");
		this->servers_config.push_back(configContent.substr(start, end - start + 1));
		this->nb_servers++;
		start = end + 1;
	}
}

//Encuentra la posicion inicial de { despues de "servidor"
size_t	Config::findStartPos(size_t start, std::string &configContent)
{
	size_t	i;

	for (i = start; configContent[i]; i++)
	{
		if (configContent[i] == 's')
			break ;
		if (!isspace(configContent[i]))
			throw Config::ConfFileException("Wrong character out of server scope{}");
	}
	if (!configContent[i])
		return std::string::npos;
	if (configContent.compare(i, 6, "server") != 0)
		throw Config::ConfFileException("Wrong character out of server scope{}");
	i += 6;
	while (configContent[i] && isspace(configContent[i]))
		i++;
	if (configContent[i] == '{')
		return i;
	else
		throw Config::ConfFileException("Wrong character out of server scope{}");
}

//Encuentra la posicion final de } despues de la posición start
size_t	Config::findEndPos(size_t start, std::string &configContent)
{
	size_t	i;
	size_t	scope;

	scope = 0;
	for (i = start + 1; configContent[i]; i++)
	{
		if (configContent[i] == '{')
			scope++;
		if (configContent[i] == '}')
		{
			if (!scope)
				return (i);
			scope--;
		}
	}
	return std::string::npos;
}

void	Config::removeFirstAndLastLine(std::string &str)
{
	size_t pos = str.find('\n');
	if (pos == std::string::npos)
		throw Config::ConfFileException("Wrong server configuration");

	str = str.substr(pos + 1);
	pos = str.rfind('\n');
	if (pos == std::string::npos)
		throw Config::ConfFileException("Wrong server configuration");
	str = str.substr(0, pos);
}

void	Config::removeInitialSpaces(std::string &str)
{

	for (size_t i = 0; i < str.length(); i++)
	{
		size_t iniPos = i;
		while (isspace(str[i]))
			i++;
		str.erase(iniPos, i - iniPos);

		i = iniPos;
		while (str[i] && str[i] != '\n')
			i++;
	}
}

std::vector<std::string> Config::getContentVec(std::string &servConf)
{
	std::vector<std::string> content;
	for (size_t i = 0; i < servConf.length(); i++)
	{
		if (servConf.substr(i, 8) == "location")
		{
			//guardar todo lo q hay dentro de {}
			size_t finalPos = servConf.find('}', i);
			if (finalPos != std::string::npos)
			{
				size_t auxPos = servConf.find('\n', finalPos);
				if (auxPos == std::string::npos)
					auxPos = servConf.size() - 1;
				while (isspace(servConf[auxPos]))
					auxPos--;
				if (auxPos != finalPos)
					throw Config::ConfFileException("Location error: problem after scope {}");
				content.push_back(servConf.substr(i, finalPos - i + 1));
			}
			else
				throw Config::ConfFileException("Location error: problem with scope {}");
			i = finalPos;
		}
		else
		{
			size_t finalPos = servConf.find('\n', i);
				if (finalPos != std::string::npos)
			{
				while (isspace(servConf[finalPos - 1]))
					finalPos--;
				content.push_back(servConf.substr(i, finalPos - i));
			}
			else
				{
				finalPos = servConf.length();
				while (isspace(servConf[finalPos - 1]))
					finalPos--;
				content.push_back(servConf.substr(i, finalPos - i));
			}
		}
		while (servConf[i] != '\n')
			i++;
	}
	return content;
}

std::vector<std::string> Config::ft_split_spaces(std::string str)
{
	std::vector<std::string> result;
	std::istringstream iss(str);
	std::string word;

	while (iss >> word)
		result.push_back(word);

	return result;
}

std::string Config::getNamePath(std::string &allLocation)
{
	size_t i = 0;
	while (allLocation[i] != ' ')
		i++;
	while (allLocation[i] == ' ')
		i++;
	size_t finalPos = i;
	while (allLocation[finalPos] != ' ' && allLocation[finalPos] != '\n' && allLocation[finalPos] != '{')
		finalPos++;
	return allLocation.substr(i, finalPos - i);
}

std::vector<std::string> Config::getLocationArgs(std::string &allLocation)
{
	size_t i = allLocation.find(' ');
	while (allLocation[i] && isspace(allLocation[i])) // salto espacios de antes del nombre de location
		i++;
	while (allLocation[i] && !isspace(allLocation[i]) && allLocation[i] != '{') //salto nombre de location
		i++;
	while (allLocation[i] && isspace(allLocation[i])) //salto espacios después del nombre de location
		i++;
	if (allLocation[i] != '{')
		throw ConfFileException("Wrong location configuration");
	i++; //me salto '{'
	while (isspace(allLocation[i])) //voy al comienzo del primer argumento
		i++;
	//aqui ya tengo en i el comienzo de location
	std::vector<std::string> args;
	for (size_t z = i; allLocation[z]; z++)
	{
		while (allLocation[z] != '\n')
		{
			if (allLocation[z] == '}')
				break ;
			z++;
		}
		if (allLocation[z] == '}')
				break ;
		if (z == std::string::npos)
		{
			args.push_back(allLocation.substr(i));
			break ;
		}
		args.push_back(allLocation.substr(i, z - i));
		i = z + 1;
	}
	return args;
}

void	Config::vectorToServer(std::vector<std::string> &content, Server &serv)
{
	std::vector<std::string>::iterator it;
	for (it = content.begin(); it != content.end(); it++)
	{
		std::string param = *it;

		if (param.substr(0, 6) == "listen" && param[6] == ' ')
		{
			std::string auxPort = param.substr(7);
			removeInitialSpaces(auxPort);
			serv.setPort(auxPort);
			// std::cout << "PORT: " << serv.getPort() << std::endl;
		}
		else if (param.substr(0, 11) == "server_name" && param[11] == ' ')
		{
			std::string auxName = param.substr(12);
			removeInitialSpaces(auxName);
			serv.setServerName(auxName);
			// std::cout << "SERVER_NAME:" << serv.getServerName() << std::endl;
		}
		else if (param.substr(0, 4) == "host" && param[4] == ' ')
		{
			std::string auxHost = param.substr(5);
			removeInitialSpaces(auxHost);
			//std::cout << "HOST:" << auxHost << std::endl;
			serv.setHost(auxHost);
			//std::cout << "HOST:" << serv.getHost() << std::endl;
		}
		else if (param.substr(0, 4) == "root" && param[4] == ' ')
		{
			std::string auxRoot = param.substr(5);
			removeInitialSpaces(auxRoot);
			serv.setRoot(auxRoot);
			// std::cout << "ROOT:" << serv.getRoot() << std::endl;
		}
		else if (param.substr(0, 20) == "client_max_body_size" && param[20] == ' ')
		{
			std::string auxSize = param.substr(21);
			removeInitialSpaces(auxSize);
			serv.setClientMaxBodySize(auxSize);
			// std::cout << "CLIENT_MAX_BODY_SIZE:" << serv.getClientMaxBodySize() << std::endl;
		}
		else if (param.substr(0, 5) == "index" && param[5] == ' ')
		{
			std::string auxIndex = param.substr(6);
			removeInitialSpaces(auxIndex);
			serv.setIndex(auxIndex);
			// std::cout << "INDEX:" << serv.getIndex() << std::endl;
		}
		else if (param.substr(0, 9) == "autoindex" && param[9] == ' ')
		{
			std::string auxAutoIndex = param.substr(10);
			removeInitialSpaces(auxAutoIndex);
			serv.setAutoindex(auxAutoIndex);
			// std::cout << "AUTOINDEX:" << serv.getAutoindex() << std::endl;
		}
		else if (param.substr(0, 10) == "error_page" && param[10] == ' ')
		{
			std::string auxErrorPages = param.substr(11);
			removeInitialSpaces(auxErrorPages);
			std::vector<std::string> auxErrors = ft_split_spaces(auxErrorPages);
			serv.setErrorPages(auxErrors);
			// std::map<unsigned short, std::string>::const_iterator it;
			// for (it = serv.getErrorPages().begin(); it != serv.getErrorPages().end(); ++it)
			// {
			// 	if (!it->second.empty())
			// 		std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
			// }
			// std::cout << std::endl;
		}
		else if (param.substr(0, 8) == "location" && param[8] == ' ')
		{
			std::string nameLoc = getNamePath(param);
			std::vector<std::string> args = getLocationArgs(param);
			serv.setLocation(nameLoc, args);
			// std::cout << "." << nameLoc << "." << std::endl << std::endl;
		}
		else
			throw ConfFileException("Undefinded parameter \"" + param + "\"");
	}
	if (serv.getRoot().empty())
	{
		std::string auxRoot = getcwd(NULL, 0);
		serv.setRoot((auxRoot + ";").c_str());
	}
	if (serv.getServerName().empty())
		serv.setServerName("localhost;");
	if (serv.getPort() == 0)
	 	serv.setPort("80;");
	serv.setDefaultErrorWebs();
}

void Config::saveServers()
{
	std::vector<std::string>::iterator it;
	std::vector<short> allPorts;
	for (it = this->servers_config.begin(); it != this->servers_config.end(); it++)
	{
		std::string servConf = *it;
		removeFirstAndLastLine(servConf);
		removeInitialSpaces(servConf);
		std::vector<std::string> content = getContentVec(servConf);

		Server	newServ;
		vectorToServer(content, newServ);
		bool found_root = false;
		std::vector<Location>::const_iterator it;
		allPorts.push_back(newServ.getPort());
		for (it = newServ.getLocations().begin(); it != newServ.getLocations().end(); it++)
		{
			if (it->getPath() == "/")
				found_root = true;
		}
		if (found_root == false)
			throw Config::ConfFileException("Could not find location \"/\"");
		this->servers.push_back(newServ);
	}
	std::vector<short>::const_iterator it2;
	for (it2 = allPorts.begin(); it2 != allPorts.end(); it2++)
	{
		std::vector<short>::const_iterator it_aux;
		for (it_aux = it2 + 1; it_aux != allPorts.end(); it_aux++)
			if (*it_aux == *it2)
				throw Config::ConfFileException("Can not set the same 2 ports in Configuration file");
	}
}

//printear el vector de configuraciones de todos los servidores
void Config::printAllServerConfig()
{
	for (std::vector<std::string>::iterator it = this->servers_config.begin(); it != servers_config.end(); ++it)
	{
		removeFirstAndLastLine(*it);
		removeInitialSpaces(*it);
        std::cout << *it << std::endl;
    }
}

//destructor
Config::~Config()
{
}

Server	*Config::getServer(int i)
{
	if (i < this->nb_servers)
		return &(this->servers[i]);
	return NULL;
}

int	Config::getNbServers()
{
	return this->nb_servers;
}