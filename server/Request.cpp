#include "../inc/Request.hpp"
#include "../inc/initialization.hpp"
#include "../inc/cgi.hpp"

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

void Request::extractRequestVariables(std::map<std::string, std::string>&RequestMap)
{
	size_t i;
	string delim;

	// i = RequestMap["Body"].find("Content-Disposition: ") + 21;
	delim = RequestMap["Body"].substr(0, RequestMap["Body"].find_first_of("\n"));
	addMethodeInMap("Content-Disposition: ", RequestMap["Body"], RequestMap);
	addMethodeInMap("name", RequestMap["Body"], RequestMap);
	addMethodeInMap("filename", RequestMap["Body"], RequestMap);
	addMethodeInMap("Content-Type: ", RequestMap["Body"], RequestMap);
	addMethodeInMap(" ", RequestMap["Body"], RequestMap);
	i = RequestMap["Body"].find("Content-Type: ");
	RequestMap["Body"].erase(0, i);
	cout << "CHecking Body after first erase: " << RequestMap["Body"] << endl << endl;
	i = RequestMap["Body"].find_first_of("\n") + 1;
	RequestMap["Body"].erase(0, i + 1);
	cout << "CHecking Body before extracting fileData: " << RequestMap["Body"] << endl;
	RequestMap["File-Data"].substr(0, RequestMap["Body"].size() - delim.size());
	cout << "FILEDATA: " << RequestMap["File-Data"] << endl;
	cout << endl;
}

void Request::createMap(string &RequestStr, std::map<string, string> &RequestMap, serverConfig &serv)
{
	size_t i_end = 0;
	int query = 0;
	if (this->_RequestStr.compare (0, 4, "GET ") == 0)
	{
		RequestMap["Method"]= "GET";
		RequestStr.erase(0, 4);
	}
	else if (this->_RequestStr.compare (0, 7, "DELETE ") == 0)
	{
		RequestMap["Method"]= "DELETE";
		RequestStr.erase(0, 7);
	}
	else if (this->_RequestStr.compare (0, 5, "POST ") == 0)
	{
		RequestMap["Method"]= "POST";
		RequestStr.erase(0, 5);
	}
	i_end = RequestStr.find_first_of(" ?", 0);
	if (RequestStr[i_end] == '?')
		query = 1;
	RequestMap["File"]= RequestStr.substr(0, i_end);
	if(RequestMap["File"][0] != '/')
	{
		//error page internal server problem
		return;
	}
	serv.setFile(RequestMap["File"]);
	RequestStr.erase(0, i_end + 1);
	if (query == 0)
	{
		i_end = RequestStr.find_first_of(" \r\n", 0);
		if(RequestStr[i_end] == '\r' || RequestStr[i_end] == '\n')
		{
			RequestMap["Protocol"]= RequestStr.substr(0, i_end);
			RequestStr.erase(0, i_end + 1);
			addMethodeInMap("Host: ", RequestStr, RequestMap);
			addMethodeInMap("Connection: ", RequestStr, RequestMap);
			addMethodeInMap("Content-Type: ", RequestStr, RequestMap);
			addMethodeInMap("Content-Length: ", RequestStr, RequestMap);
		}
		else
		{
			//bad input
			return;
		}
	}
	else if( query == 1)
	{
		while(1)
		{
			i_end = RequestStr.find_first_of("& \n\r", 0);
			if (RequestStr[i_end] == '&' || RequestStr[i_end] == ' ')
			{
				size_t i_middle = RequestStr.find_first_of("=", 0);
				std::string index = RequestStr.substr(0, i_middle);
				std::string value = RequestStr.substr(i_middle + 1, i_end - i_middle -1);
				RequestMap[index]= value;
				RequestStr.erase(0, i_end + 1);
			}
			else if(RequestStr[i_end] == '\r' || RequestStr[i_end] == '\n')
			{
				RequestMap["Protocol"]= RequestStr.substr(0, i_end);
				RequestStr.erase(0, i_end + 1);
				addMethodeInMap("Host: ", RequestStr, RequestMap);
				addMethodeInMap("Connection: ", RequestStr, RequestMap);
				addMethodeInMap("Content-Type: ", RequestStr, RequestMap);
				addMethodeInMap("Content-Length: ", RequestStr, RequestMap);
				return;
			}
			else
			{
				//error bad input
				return;
			}
		}
	}
	//add body in request map for post
	i_end = RequestStr.find("\r\n\r\n", 0);
	RequestStr.erase(0, i_end + 2);
	RequestMap["Body"] = RequestStr.substr(0, i_end);
	// extractRequestVariables(RequestMap);
	std::cout << "Request Body: " << RequestMap["Body"] << std::endl;
	std::cout << iscgi(RequestMap["File"], ".py") << std::endl;
	if(iscgi(RequestMap["File"], ".py") == 1 )
	{
		RequestMap["Extension"] = "py";
		RequestMap["CGI_Path"] = "/usr/bin/python3";
	}
	else if (iscgi(RequestMap["File"], ".sh") == 1)
	{
		RequestMap["Extension"] = "sh";
		RequestMap["CGI_Path"] = "/bin/bash";
	}
	else if (iscgi(RequestMap["File"], ".php") == 1)
	{
		RequestMap["Extension"] = "php";
		RequestMap["CGI_Path"] = "/usr/bin/php";
	}
}

void	Request::RequestExecution(serverConfig &serv, Response &response, std::string text, std::map<string, string> &RequestMap)
{
	// std::cout << this->_RequestStr << std::endl;
	// std::cout << this->_RequestStr.compare (0, 7, "DELETE ") << std::endl;
	for(std::map<string, string>::iterator b = RequestMap.begin(); b != RequestMap.end(); b++)
		cout << "->" << b->second << "<-" << endl;
	if(RequestMap.find("Extension") != RequestMap.end())
		executecgi(RequestMap);
	if (RequestMap["Method"] == "GET")
	{
		
		std::cout << "get" << std::endl;
		if(getMethode(response.getResponse_data(), RequestMap, serv, response, text) != 0)
		{
			//TODO gérer les erreurs de GET
			std::cout << "PROBLEM WITH GET";
		}
	}
	else if (RequestMap["Method"] == "DELETE")
	{
		std::cout << "delete" << std::endl;
		if(deleteMethode(RequestMap, serv) != 0)
		{
			//TODO gérer les erreurs de DELETE
			std::cout << "PROBLEM WITH DELETE";
		}
	}
	else if (RequestMap["Method"] == "POST")
	{
			std::cout << "post" << std::endl;
			if(postMethode(RequestMap, serv) != 0)
			{
				//TODO gérer les erreurs de POST
				std::cout << "PROBLEM WITH POST";
			}
	}
	else
	{
		std::cout << "else" << std::endl;
	}
}

string &Request::getRequestStr()
{
	return(this->_RequestStr);
}
