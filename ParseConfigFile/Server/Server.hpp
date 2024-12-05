#pragma once

# include "../../webserv.hpp"
# include "../Location/Location.hpp"

class Location;

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
		pollfd									_pollfd;

	public:
		//Constructor & destructor
			Server();
			Server(Server const &copy);
			~Server();

		//public methods
			void						initErrorWebs();

			void						tryLocation(Location &location);
			bool						isValidHost(std::string hostName) const;

			void						endWithSemicolon(std::string &token);

			static int					typeOfPath(std::string const path);
			bool						existParam(std::string &str, size_t pos);
			std::string					paramReturn(std::string &src, int pos);
			std::vector<std::string>	splitSpace(std::string &methods);
			void						parseExtensions(std::vector<std::string> &extensions, std::string &auxExt);
			void						parseCgiPath(std::vector<std::string> &paths, std::string &auxPaths);
			void						startServer(void);
			int 						createServerSocket();
			bool						emptyErrorWebs();
			void						setDefaultErrorWebs();

		//setters
			void	setServerName(std::string server_name);								// hecho
			void	setHost(std::string host);											// hecho
			void	setRoot(std::string root);											// hecho
			void	setFd(int &fd);
			void	setPort(std::string port);											// hecho
			void	setClientMaxBodySize(std::string maxBodySize);						// hecho
			void	setErrorPages(std::vector<std::string> error_webs);					// hecho
			void	setIndex(std::string &index);										// hecho
			void	setLocation(std::string &name, std::vector<std::string> &args);		// hecho
			void	setAutoindex(std::string &autoindex);								// hecho

		//getters
			const std::string &getServerName();
			const pollfd &getPollfd();
			const uint16_t &getPort();
			const int &getListenFd();
			const in_addr_t &getHost();
			const unsigned long &getClientMaxBodySize();
			std::vector<Location> &getLocations();
			const std::string &getRoot();
			const std::map<unsigned short, std::string> &getErrorPages();
			const std::string &getIndex();
			const bool &getAutoindex();
			const std::string &getPathErrorPage(unsigned int key);
			const std::vector<Location>::iterator getLocationKey(std::string key);

		//Exceptions
			class ServerErrorException: public std::exception
			{
				private:
					std::string message;
				public:
					ServerErrorException(std::string const &msg) throw()
					{
						message = "Server error: " + msg;
					}
					virtual const char *what() const throw()
					{
						return (message.c_str());
					}
					virtual ~ServerErrorException() throw() {}
			};
};
