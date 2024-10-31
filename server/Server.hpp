#pragma once

# include "../webserv.hpp"
# include "../Location/Location.hpp"

class Server //vector de servers en config
{
	private:
		uint16_t								port;
		in_addr_t								host;
		std::string								serv_name;
		std::string								root;
		unsigned long							client_max_body_size;
		std::string								index;
		bool									autoindex;
		std::map<unsigned short, std::string>	errorWebs;
		std::vector<Location>					locations;
		struct sockaddr_in						server_address;
		int										listen_fd;

	public:
		//Constructor & destructor
			Server();
			Server(Server const &copy);
			~Server(void);

		//public methods
			void	tryErrorWebs();
			void	tryLocation(Location &location) const;
			bool	isValidHost(std::string hostName) const;

		//setters
			void	setServerName(std::string const &server_name);
			void	setHost(std::string const &host);
			void	setRoot(std::string const &root);
			void	setFd(int const &fd);
			void	setPort(std::string const &port);
			void	setClientMaxBodySize(std::string const &maxBodySize);
			void	setErrorPages(std::vector<std::string> const &error_webs);
			void	setIndex(std::string const &index);
			void	setLocation(std::string const &name, std::vector<std::string> const &args);
			void	setAutoindex(std::string const &autoindex);

		//getters
};
