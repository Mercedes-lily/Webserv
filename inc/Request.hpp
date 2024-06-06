#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "serverConfig.hpp"
#include "Response.hpp"

using std::string;
using std::exception;
using std::cout;
using std::endl;
using std::remove;

class Request
{
private:
	char _req[10000];
	string _RequestStr;
public:
	Request(void);
	Request(Request const& src);
	~Request(void);
	Request& operator=(Request const& rhs);
	int	RequestReception(serverConfig &serv, pollfd *pollfds, int fd);
	void	RequestExecution(serverConfig &serv, Response &response, string text, std::map<string, string> &RequestMap);
	string &getRequestStr();
	void createMap(string &RequestStr, std::map<string, string> &RequestMap, serverConfig &serv);
	void extractRequestVariables(std::map<std::string, std::string>&RequestMap);
};

void addMethodeInMap(string str, string &RequestStr, std::map<string,string> &RequestMap);
int getMethode(string &response_data, std::map<string, string> &RequestMap, serverConfig &serv, Response &response, string text);
int deleteMethode(std::map<string, string> &RequestMap, serverConfig &serv);
int postMethode(std::map<string, string> &RequestMap, serverConfig &serv);
#endif