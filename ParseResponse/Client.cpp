# include "Client.hpp"

Client::Client(sockaddr_in sock, pollfd fd)
{
	this->_socket = sock;
	this->_pollfd = fd;
	this->nb_req = 0;
}

Client::Client(Client const &copy)
{
	this->_socket = copy._socket;
	this->_pollfd = copy._pollfd;
	this->clientRequests = copy.clientRequests;
	this->nb_req = copy.nb_req;
}

Client::~Client()
{
}

void	Client::addRequest(std::string req)
{
	if (this->nb_req >= 1)
	{
		if (this->clientRequests[nb_req - 1].fragmented)
		{
			int aux_len = this->clientRequests[nb_req - 1].content_length - req.length();
			// std::cout << std::endl << aux_len << std::endl;
			std::string finalReq = clientRequests[nb_req - 1].rawRequest + req;
			this->clientRequests.pop_back();

			HttpRequest newReq;
			if (aux_len > 0)
			{
				newReq.setHttpReq(finalReq, false, aux_len);
			}
			else
			{
				newReq.setHttpReq(finalReq, true, 0);
			}
			this->clientRequests.push_back(newReq);
		}
		else
		{
			HttpRequest newReq;
			newReq.setHttpReq(req, false, -1);
			this->clientRequests.push_back(newReq);
			this->nb_req++;
		}
		//comprobar si el anterior es fragmentado
	}
	else
	{
		HttpRequest newReq;
		newReq.setHttpReq(req, false, -1);
		this->clientRequests.push_back(newReq);
		this->nb_req++;
	}
}