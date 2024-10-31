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
	this->errorWebs.insert(std::make_pair(403, "")); //Foridden
	this->errorWebs.insert(std::make_pair(404, "")); //Not found
	this->errorWebs.insert(std::make_pair(405, "")); //Method not allowed
	this->errorWebs.insert(std::make_pair(406, "")); //Not acceptable
	this->errorWebs.insert(std::make_pair(408, "")); //Request timeout
	this->errorWebs.insert(std::make_pair(429, "")); //Too Many Request
	this->errorWebs.insert(std::make_pair(500, "")); //Internal server error
	this->errorWebs.insert(std::make_pair(501, "")); //Not implemented
	this->errorWebs.insert(std::make_pair(502, "")); //Bad Gateway
	this->errorWebs.insert(std::make_pair(503, "")); //Service unaviable
	this->errorWebs.insert(std::make_pair(504, "")); //Gateway timeout
	this->errorWebs.insert(std::make_pair(505, "")); //HTTP version not supported
}

void	Server::tryLocation(Location &location) const
{
}

bool	Server::isValidHost(std::string hostName) const
{
}

void	Server::endWithSemicolon(std::string const &token)//comprueba que acaba en ;
{
	size_t	pos = token.find(';');
	if (pos == std::string::npos)
		throw Config::ConfFileException("Token \"" + token + "\" must end with ;");

	for (int i=pos; i < token.size(); i++)
	{
		if (!isspace(token[i]))
			throw Config::ConfFileException("Wrong token \"" + token + "\"");
	}
}

bool	Server::checkLocations() const
{
}

/****************************SETTERS*****************************/

void	Server::setServerName(std::string const &server_name)
{
	endWithSemicolon(server_name);
	this->serv_name = server_name;
}

void	setHost(std::string const &host)
{

}

void	setRoot(std::string const &root)
{

}

void	setFd(int const &fd)
{

}

void	setPort(std::string const &port)
{

}

void	setClientMaxBodySize(std::string const &maxBodySize)
{

}

void	setErrorPages(std::vector<std::string> const &error_webs)
{

}

void	setIndex(std::string const &index)
{

}

void	setLocation(std::string const &name, std::vector<std::string> const &args)
{

}

void	setAutoindex(std::string const &autoindex)
{

}

/****************************GETTERS*****************************/