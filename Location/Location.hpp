#pragma once

# include "../webserv.hpp"

class Location
{
	private:
		std::string			path;
		std::string			root;
		std::string			index;
		bool				autoindex;
		std::vector<short>	methods;
		std::string			returned;
		std::string			alias;


	public:
};