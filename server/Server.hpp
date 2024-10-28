#pragma once

# include "../webserv.hpp"

class Server
{
	private:
		//constructor
			Server(void);

		//attributes
			std::map<long, std::string>requests;
			std::string		rootDirectory;
			unsigned int	host;
			int				port;

	public:
		//Constructor & destructor
			Server(std::string const &fileConfig);
			Server(Server const &copy);
			~Server(void);

		//Assignment operator
			Server const &operator=(Server const &copy);

		//getters
			int	const &getPort(void) const;
			std::string const &getRootDirecctory(void) const;
};