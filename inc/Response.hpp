#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "serverConfig.hpp"

#include <string>
#include <iostream>
using std::string;
using std::exception;
using std::cout;
using std::endl;
using std::remove;

class Response
{
private:
	string	_response_data;
	string	_type;
	string	_code;
	int		_len;
public:
	Response(void);
	Response(Response const& src);
	~Response(void);
	Response& operator=(Response const& rhs);
	string	&getResponse_data();
	string	&getCode(void);
	string& getType(void);
	void	setCode(string Code);
	void	setLen(int Len);
	void	setType(string Type);
	void	setResponse_data(string Response_data);
	int		getLen(void);
};

const string &responseToClient(serverConfig &serv, Response &response, string &text);

#endif