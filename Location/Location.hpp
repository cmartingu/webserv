#pragma once

# include "../webserv.hpp"

# define MAX_REQUEST_LENGTH 30000000

class Location
{
	private:
		std::string					path;
		std::string					root;
		std::string					index;
		bool						autoindex;
		std::vector<short>			methods;
		std::string					returnPath;
		std::string					alias;
		std::vector<std::string>	cgi_path;
		std::vector<std::string>	cgi_ext;
		unsigned long				client_max_body_size;

	public:
		//constructor & destructor
			Location();
			~Location();

		//setters
			void setPath(std::string parametr);
			void setRootLocation(std::string parametr);
			void setMethods(std::vector<std::string> methods);
			void setAutoindex(std::string parametr);
			void setIndexLocation(std::string parametr);
			void setReturn(std::string parametr);
			void setAlias(std::string parametr);
			void setCgiPath(std::vector<std::string> path);
			void setCgiExtension(std::vector<std::string> extension);
			void setMaxBodySize(std::string parametr);
			void setMaxBodySize(unsigned long parametr);

		//getters
			const std::string &getPath() const;
			const std::string &getRootLocation() const;
			const std::vector<short> &getMethods() const;
			const bool &getAutoindex() const;
			const std::string &getIndexLocation() const;
			const std::string &getReturn() const;
			const std::string &getAlias() const;
			const std::vector<std::string> &getCgiPath() const;
			const std::vector<std::string> &getCgiExtension() const;
			const std::map<std::string, std::string> &getExtensionPath() const;
			const unsigned long &getMaxBodySize() const;
};