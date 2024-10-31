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
	this->tryErrorWebs();
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