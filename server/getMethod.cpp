#include "../inc/Request.hpp"
#include "../inc/initialization.hpp"

int getMethode(string &response_data, std::map<string, string> &RequestMap, serverConfig &serv, Response &response, std::string text)
{
	struct stat sb;
	//print map
	for(std::map<string, string>::iterator b = RequestMap.begin(); b != RequestMap.end(); b++)
		cout << "->" << b->second << "<-" << endl;
	//TODO verify the information in RequestMap
	if(RequestMap.find("Protocol")->second.compare("HTTP/1.1") != 0)
	{
		cout << "Problem 1" << endl;
		return(1); // TODO definir des code d'erreur
	}
	if(RequestMap.find("Connection") != RequestMap.end())
	{
		if(RequestMap.find("Connection")->second.compare("close") != 0 &&
			RequestMap.find("Connection")->second.compare("keep-alive") != 0)
			{
				cout << "Problem 2";
				return(1); // TODO definir des code d'erreur
			}
	}
	/*
	if(RequestMap.find("Host") != string::npos)
	{
		it_vec it = serverConfigConfig.begin();
		for(vector<string>::iterator it = serverConfigConfig.begin(); it != serverConfigConfig.end(); it++)
		{
			if(RequestMap.find("Host")->second.compare("HTTP/1.1"))
				break;
		}
		if(it = serverConfigConfig.end())
			return(1);  // TODO definir des code d'erreur
	}
	*/
	//verify file access
	//TODO check for the good file
	//TODO check if File is compose with multiple informations
	if (RequestMap.find("File")->second.back() == '/')
	{
		if(RequestMap.find("File")->second.compare("/") == 0)
		{
			if(access("server/default.html", R_OK) < 0)
			{
				cout << "default.html" << " not found" << endl;
				return (1);
			}
			std::fstream html_data;
			html_data.open("server/default.html");
			std::string buff_html;
			while(std::getline(html_data, buff_html))
			{
				cout << "-> " << buff_html << endl;
				response_data += buff_html;
			}
			cout << "buff_html-> " << buff_html << endl;
			cout << endl << endl;
			cout << "-----------RESPONSE-----------" << endl << endl;
			cout << "response_data-> " << response_data << endl;
			responseToClient(serv, response, text);
			cout << "-----------RESPONSE-----------" << endl << endl;

			//Envoie de la réponse//
			send(serv.getSocket_client(), text.c_str(), text.size(), 0);
			return(0);
		}
		else
		{
			if (stat(RequestMap.find("File")->second.c_str(), &sb) == -1)
			{
				cout << "stat failed" << endl;
				return (1);
			}
			if (S_ISDIR(sb.st_mode))
			{
				string defaultDirectory = "server" + RequestMap.find("File")->second + "default.html";
				if(access(defaultDirectory.c_str(), R_OK) < 0)
				{
					//TODO listing function
				}
				else
				{
					std::fstream html_data;
					html_data.open(defaultDirectory);
					std::string buff_html;
					while(std::getline(html_data, buff_html))
					{
						cout << "-> " << buff_html << endl;
						response_data += buff_html;
					}
					// cout << "buff_html-> " << buff_html << endl;
					cout << "response_data-> " << response_data << endl;
					responseToClient(serv, response, text);
					//Envoie de la réponse//
					send(serv.getSocket_client(), text.c_str(), text.size(), 0);
					return(0);
				}
			}
			else
			{
				cout << "invalid directory" << endl;
				return (1);
			}
		}
	}
	else
	{
		if(RequestMap.find("File")->second.front() == '/')
			RequestMap.find("File")->second = "server" + RequestMap.find("File")->second;
		else
			RequestMap.find("File")->second = "server/" + RequestMap.find("File")->second;
		if(access( RequestMap.find("File")->second.c_str(), R_OK) < 0)
		{
			cout << RequestMap.find("File")->second << " not found" << endl;
			return (1);
		}
		std::fstream html_data;
		html_data.open(RequestMap["File"]);
		html_data.seekg(0, html_data.end);
		response.setLen(html_data.tellg());
		html_data.seekg(0, html_data.beg);
		std::string buff_html;
		while(std::getline(html_data, buff_html))
		{
			// cout << "-> " << buff_html << endl;
			response_data += buff_html;
		}
		// cout << "buff_html-> " << buff_html << endl;
		// cout << "response_data-> " << response_data << endl;
		responseToClient(serv, response, text);
		//Envoie de la réponse//
		send(serv.getSocket_client(), text.c_str(), text.size(), 0);
		return(0);
	}
	responseToClient(serv, response, text);
	//Envoie de la réponse//
	send(serv.getSocket_client(), text.c_str(), text.size(), 0);
	return(0);
}
