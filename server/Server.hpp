#pragma once

# include "../webserv.hpp"

class Server
{
	private:
		uint16_t	port;
		in_addr_t	host;
		std::string	serv_name;
		std::string	root;
		unsigned long	max_body_size;

		std::string		index;//habra q ver para q
		bool			autoindex;
		std::map<unsigned short, std::string> errorWebs;
		struct sockaddr_in	server_address;
		

	public:
		//Constructor & destructor
			Server();
			Server(Server const &copy);
			~Server(void);

		//getters
			int	const &getPort(void) const;
			std::string const &getRootDirecctory(void) const;
};