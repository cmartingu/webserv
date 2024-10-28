#pragma once

# include "../webserv.hpp"

class Config
{
	private:
		//constructor
			Config();
	public:
		//constructors & destructors
			Config(std::string const &defaultPath);
			Config(Config const &copy);
			~Config();

		//exceptions
			class NotExixtingPathException: public std::exception
			{
				public:
					virtual const char *what() const throw()
					{
						return ("Configuration file error: Not existing file");
					}
			};
};

std::string	getConfFile(std::string const &defaultPath);