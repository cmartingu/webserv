#pragma once

# include "../webserv.hpp"
# include "./Server/Server.hpp"

class Server;

class Config
{
	private:
		int	nb_servers;
		std::vector<std::string> servers_config;
		std::vector<Server> servers;
		// fd_set				_fd_set;

	public:
		//constructors & destructors
			Config(std::string const &defaultPath);
			Config(Config const &copy);
			~Config();

		//public methods
			std::string					getConfFile(std::string const &defaultPath);
			void						removeComments(std::string &content);
			void						saveConfigs(std::string &configContent);
			size_t						findStartPos(size_t start, std::string &configContent);
			size_t						findEndPos(size_t start, std::string &configContent);

			void						removeFirstAndLastLine(std::string &str);
			void						removeInitialSpaces(std::string &str);
			std::vector<std::string>	getContentVec(std::string &servConf);
			void						vectorToServer(std::vector<std::string> &content, Server &serv);
			std::vector<std::string>	ft_split_spaces(std::string str);
			void						saveServers();

			std::vector<std::string>	getLocationArgs(std::string &allLocation);
			std::string					getNamePath(std::string &allLocation);
			Server						*getServer(int i);
			int							getNbServers();

		//print configs
			void	printAllServerConfig();

		//exceptions
			class ConfFileException: public std::exception
			{
				private:
					std::string message;
				public:
					ConfFileException(std::string const &msg) throw()
					{
						message = "Configuration file error: " + msg;
					}
					virtual const char *what() const throw()
					{
						return (message.c_str());
					}
					virtual ~ConfFileException() throw() {}
			};
};
