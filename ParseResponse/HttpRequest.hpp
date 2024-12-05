#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

# include "../webserv.hpp"
#include "../ParseConfigFile/Config.hpp"

class Config;

class Location;

class HttpRequest
{
	private:                               // Cuerpo de la solicitud (para POST, PUT, etc.)
		void parseRequestLine(const std::string& requestLine);
		void parseHeaders(std::istream& stream, int ct_len);
		void getHost();

	public:
		std::string rawRequest;
		Server *server;
		Location *location;
		std::string method;                              // Método HTTP (GET, POST, DELETE)
		std::string path;                                // Ruta (por ejemplo, /upload)
		std::string version;                             // Versión del protocolo HTTP
		std::string queryString;                         // Parámetros de consulta (si existen)
		std::map<std::string, std::string> headers;      // Encabezados HTTP
		std::string body; 
		int port;
		bool fragmented;
		int	content_length;
		std::string host;
		std::string status;                              // Estado de la respuesta (200 OK, 404 Not Found)
		std::map<std::string, std::string> responseHeaders; // Encabezados de la respuesta
		std::string responseBody;                        // Cuerpo de la respuesta
		// Constructor: toma una solicitud HTTP completa como entrada
		// explicit HttpRequest(const std::string& rawRequest);
		void setHttpReq(const std::string& rawRequest, bool second, int ct_len);
		HttpRequest();
		void printRequestDetails() const;
		std::string getFileName();

		//Chequeo de location/server
		bool checkServer(Config &conf);
		bool isMethodAllowed(const std::vector<std::string>& allowedMethods) const;
		bool properBodySize();
		bool validateLocation();
		void parseHttpRequest();


		//Métodos
		void methodGet();
		void methodPost();
		void methodDelete();

		// Respuesta
		void setResponse(const std::string& newStatus, const std::string& content, const std::string& contentType);
		std::string generateResponse() const;
		void generateAutoIndex(const std::string& directoryPath);
		std::string getPathLocation(std::string path) const;
		std::string executeCGI(const std::string &cgiPath, const std::string &scriptPath);

		//getter
			int	getContentLength();
};

#endif