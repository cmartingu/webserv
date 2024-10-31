#pragma once

# include "../webserv.hpp"
# include "../Location/Location.hpp"
# include "../Parse/Config.hpp"

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
			~Server();

		//public methods
			void	initErrorWebs();

			void	tryLocation(Location &location) const;
			bool	isValidHost(std::string hostName) const;

			void	endWithSemicolon(std::string const &token);
			bool	checkLocations() const;

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
			const std::string &getServerName();
			const uint16_t &getPort();
			const in_addr_t &getHost();
			const size_t &getClientMaxBodySize();
			const std::vector<Location> &getLocations();
			const std::string &getRoot();
			const std::map<short, std::string> &getErrorPages();
			const std::string &getIndex();
			const bool &getAutoindex();
			const std::string &getPathErrorPage(short key);
			const std::vector<Location>::iterator getLocationKey(std::string key);
};
