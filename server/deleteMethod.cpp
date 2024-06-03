#include "../inc/Request.hpp"
#include "../inc/initialization.hpp"
#include <cstdio>

void createDeleteMap(string &RequestStr, std::map<string, string> &RequestMap, serverConfig &serv)
{
	size_t i_end = 0;
	int query = 0;
	RequestMap["Methode"]= "DELETE";
	RequestStr.erase(0, 7);
	i_end = RequestStr.find_first_of(" ?", 0);
	if (RequestStr[i_end] == '?')
		query = 1;
	RequestMap["File"]= RequestStr.substr(0, i_end);
	if (RequestMap["File"][0] == '/')
		RequestMap["File"]= serv.getRoot() + "upload" + RequestStr.substr(0, i_end);
	else
		RequestMap["File"]= serv.getRoot() + "upload/" + RequestStr.substr(0, i_end);
	serv.setFile(RequestMap["File"]);
	RequestStr.erase(0, i_end + 1);
	if (query == 0)
	{
		i_end = RequestStr.find_first_of(" \r\n", 0);
		i_end = RequestStr.find_first_of(" \r\n", 0);
		if(RequestStr[i_end] == '\r' || RequestStr[i_end] == '\n')
		{
			RequestMap["Protocol"]= RequestStr.substr(0, i_end);
			RequestStr.erase(0, i_end + 1);
			addMethodeInMap("Host: ", RequestStr, RequestMap);
			addMethodeInMap("Content-Type: ", RequestStr, RequestMap);
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
							std::cout << index << "   " << value << std::endl;
				RequestMap[index]= value;
				RequestStr.erase(0, i_end + 1);
			}
			else if(RequestStr[i_end] == '\r' || RequestStr[i_end] == '\n')
			{
				RequestMap["Protocol"]= RequestStr.substr(0, i_end);
				RequestStr.erase(0, i_end + 1);
				addMethodeInMap("Content-Type: ", RequestStr, RequestMap);
				addMethodeInMap("Host: ", RequestStr, RequestMap);
				return;
			}
			else
			{
				//error bad input
				return;
			}
		}
	}
}

int deleteMethode(string &RequestStr, string &response_data, std::map<string, string> &RequestMap, serverConfig &serv, Response &response)
{
	(void) response_data;
	(void) response;
	struct stat sb;
	createDeleteMap(RequestStr, RequestMap, serv);

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