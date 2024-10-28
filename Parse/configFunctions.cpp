#include "Config.hpp"

std::string	getConfFile(std::string const &defaultPath)// devuelve el contenido de defaultPath en string
{
	if (defaultPath.empty() || defaultPath.length() == 0)
		throw Config::ConfFileException("Not existing file");
	
	std::ifstream configfd(defaultPath.c_str());
	if (!configfd || !configfd.is_open())
		throw Config::ConfFileException("Not existing file");

	std::string content, line;
	while (std::getline(configfd, line))
		content += line + '\n';
	return content;
}

void	removeComments(std::string &content)
{
	size_t	pos = content.find('#');
	while (pos != std::string::npos)
	{
		size_t	final_pos = content.find('\n', pos);
		content.erase(pos, final_pos - pos);
		pos = content.find('#');
	}
}
