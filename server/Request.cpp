#include "../inc/Request.hpp"
#include "../inc/initialization.hpp"

Request::Request(void)
{
}

Request::Request(Request const& src)
{
	*this = src;
}

Request& Request::operator=(Request const& rhs)
{
	if (this != &rhs)
	{
	}
	return *this;
}

Request::~Request(void)
{
}

int Request::RequestReception(serverConfig &serv, pollfd *pollfds, int fd)
{
	cout << "request fd " << fd << endl;
	int valread = recv(fd, this->_req, 10000, 0);
	cout << endl << endl;
	cout << "-----Request------" << endl;
	cout << this->_req << endl;
	cout << "-----Request------" << endl;
	cout << endl << endl;
	string tmp(this->_req);
	this->_RequestStr = tmp;
	//cout << "buffer: " << request << endl;
	if (valread < 0)
	{
		perror("read");
		closefile("valread -1", serv.getFd(), serv.getSocket_client());
		//TODO afficher la page d'erreur
		return (1);
	}
	else if (valread == 0)
	{
		close((pollfds + fd)->fd);
		(pollfds + fd)->fd = -1;
	}
	return (0);
}

void	Request::RequestExecution(serverConfig &serv, Response &response, std::string text)
{
	// std::cout << this->_RequestStr << std::endl;
	// std::cout << this->_RequestStr.compare (0, 7, "DELETE ") << std::endl;
	if (this->_RequestStr.compare (0, 4, "GET ") == 0)
	{
		
		std::cout << "get" << std::endl;
		std::map<string, string> getMap;
		if(getMethode(this->_RequestStr, response.getResponse_data(), getMap, serv, response, text ) != 0)
		{
			//TODO gérer les erreurs de GET
			std::cout << "PROBLEM WITH GET";
		}
	}
	else if (this->_RequestStr.compare (0, 7, "DELETE ") == 0)
	{
		std::cout << "delete" << std::endl;
		std::map<string, string> deleteMap;
		if(deleteMethode(this->_RequestStr, response.getResponse_data(), deleteMap, serv, response) != 0)
		{
			//TODO gérer les erreurs de POST
			std::cout << "PROBLEM WITH DELETE";
		}
	}
	else if (this->_RequestStr.compare (0, 5, "POST ") == 0)
	{
			std::cout << "post" << std::endl;
			/*std::map<string, string> postMap;
			if(postMethode(this->_RequestStr, response.getResponse_data(), postMap, serv, response) != 0)
			{
				//TODO gérer les erreurs de POST
				std::cout << "PROBLEM WITH POST";
			}*/
	}
	else
	{
		std::cout << "else" << std::endl;
	}
}
