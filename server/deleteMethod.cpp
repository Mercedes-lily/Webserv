#include "../inc/Request.hpp"
#include "../inc/initialization.hpp"
#include <cstdio>

int deleteMethode(std::map<string, string> &RequestMap, serverConfig &serv)
{
	struct stat sb;
	//print map
	for(std::map<string, string>::iterator b = RequestMap.begin(); b != RequestMap.end(); b++)
		cout << "->" << b->second << "<-" << endl;
	//TODO verify the information in RequestMap
	if(RequestMap.find("Protocol")->second.compare("HTTP/1.1") != 0)
	{
		cout << "Problem 1 in Delete Method" << endl;
		return(1); // TODO definir des code d'erreur
	}
	// TODO regarder si c'est le bon host 
	if (stat(RequestMap.find("File")->second.c_str(), &sb) == -1)
	{
		perror("why?...;!!!!!;");
		cout << "stat failed" << endl;
		return (1);
	}
	if (S_ISDIR(sb.st_mode))
	{
		if(RequestMap.find("File")->second.back() != '/')
			{
			cout << "Does not end with /" << endl;
			return (409);
			}
		else if(access( RequestMap.find("File")->second.c_str(), W_OK) < 0)
			{
				cout << "Access - directory - denied" << endl;
				return (403);
			}
	}
	else if(access( RequestMap.find("File")->second.c_str(), F_OK) < 0)
	{
		cout << "File does not exist" << endl;
		return (404);
	}
	//TODO attention rmove utilise le path absolu
	if(remove(RequestMap["File"].c_str()) != 0)
	{
		cout << RequestMap["File"] << " : Permission denied (remove file or valid directory)" << endl;
		return(500);
	}
	//TODO change response with the right values
	string text = "HTTP/1.1 ";
	text += "200 OK\r\nContent-Type: text/html\r\n\r\n";
	send(serv.getSocket_client(), text.c_str(), text.size(), 0);
	return(0);
}