#pragma once

# include "../webserv.hpp"
# include "HttpRequest.hpp"

class HttpRequest;

class Client
{
	public:
		sockaddr_in					_socket;
		pollfd						_pollfd;
		std::vector<HttpRequest>	clientRequests;
		int							nb_req;

		Client(sockaddr_in sock, pollfd fd);
		Client(Client const &copy);
		~Client();

		void	addRequest(std::string req);
};