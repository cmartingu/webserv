#pragma once

# include "../webserv.hpp"

class Config
{
	private:
		int	nb_servers;
		std::vector<std::string> servers_config;

	public:
		//constructors & destructors
			Config(std::string const &defaultPath);
			Config(Config const &copy);
			~Config();

		//public methods
			void	saveConfigs(std::string &configContent);
			size_t	findStartPos(size_t start, std::string &configContent);
			size_t	findEndPos(size_t start, std::string &configContent);


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

//Clean config file
std::string	getConfFile(std::string const &defaultPath);
void		removeComments(std::string &content);