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
};