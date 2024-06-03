#include "../inc/Response.hpp"
#include <sstream>

Response::Response(void)
{
}

Response::Response(Response const& src)
{
	*this = src;
}

Response& Response::operator=(Response const& rhs)
{
	if (this != &rhs)
	{
	}
	return *this;
}

Response::~Response(void)
{
}

std::string	&Response::getResponse_data(void)
{
	return (this->_response_data);
}

void	Response::setCode(string Code)
{
	this->_code = Code;
}

void	Response::setLen(int Len)
{
	this->_len = Len;
}

void	Response::setType(string Type)
{
	this->_type = Type;
}

void	Response::setResponse_data(string Response_data)
{
	this->_response_data = Response_data;
}

std::string	&Response::getCode(void)
{
	return (this->_code);
}

int	Response::getLen(void)
{
	return (this->_len);
}

std::string	&Response::getType(void)
{
	return (this->_type);
}

const string &responseToClient(serverConfig &serv, Response &response, string &text)
{
	text = "HTTP/1.1 ";
	std::ostringstream oss;
	oss << response.getLen();
	if (serv.getFile().rfind(".ico") != string::npos)
	{
		text += response.getCode() + "\r\n";
		text += "Content-Type: image/ico ";
		text += "Content-Length: ";
		text += oss.str() + "\r\n\r\n";
	}
	else if (serv.getFile().rfind(".png") != string::npos)
	{
		text += response.getCode() + "\r\n";
		text += "Content-Type: image/png ";
		text += "Content-Length: ";
		text += oss.str() + "\r\n\r\n";
	}
	else
	{
		text += "200 OK\r\nContent-Type: text/html\r\n\r\n";
		text += response.getResponse_data();
	}
	return(text);
}
