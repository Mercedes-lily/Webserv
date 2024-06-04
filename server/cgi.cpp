//test.py
#include "serverConfig.hpp"

#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>

std::string find_replace(std::string readed, std::string s1, std::string s2)
{
	size_t found = 0;
	
	while(found != std::string::npos)
	{
		found = readed.find(s1, found);
		if (found != std::string::npos)
		{
			readed.erase(found, s1.length());
			readed.insert(found, s2);
			found += s2.length();
		}
	}
	return (readed);
}

int iscgi(std::string file, std::string extension)
{
	size_t pos;
	if ((pos =file.rfind(extension, 100)) != std::string::npos)
	{
		if(file.compare(pos, extension.size() + 1, extension) == 0)
			return(1);
		return(0);
	}
	return(0);
}

//verifier dans la map si cest py php ou sh
int executecgi(std::map<string, string> &RequestMap)
{
	int pid;
	int status;
	int fd;

	std::string name;
	std::string cgipath = RequestMap["CGI_Path"];
	std::string filepath = "/Users/vst-pier/Desktop/webserv2.0/server/script/" + RequestMap["File"];  //TODO mettre le path de serv
	if (RequestMap.find("Name") != RequestMap.end())
		name = RequestMap["Name"];
	else
		name = "Sebastien";
	char *env[4];
	env[0] = new char[cgipath.length() + 1];
	std::strcpy(env[0], cgipath.c_str());
	env[1] = new char[filepath.length() + 1];
	std::strcpy(env[1], filepath.c_str());
	env[2] = new char[name.length() + 1];
	std::strcpy(env[2], name.c_str());
	env[3] = NULL;

	if((pid = fork()) == -1)
		return(500);
	if (pid == 0)
	{
		if((fd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
			return(500);
		if(dup2(fd, 1) == -1)
			return(500);
		execve(cgipath.c_str(), env, NULL);
		exit(500);
	}
	waitpid(pid, &status, 0);
	if (RequestMap["Method"] == "GET")
	{
		std::string readed;
		std::fstream htmlTemplate("server/template.html");
		if(htmlTemplate.is_open() == 0)
		{
			std::cout << "template not open" << std::endl;
			return (500);
		}
		std::ofstream cgi_html("server/cgi.html");
		std::fstream ToReplaceFile("temp.txt");
		std::string ToReplaceString;
		getline(ToReplaceFile, ToReplaceString);
		while(getline(htmlTemplate, readed))
		{
			cgi_html << find_replace(readed, "(ToReplace)", ToReplaceString);
			if(htmlTemplate.peek() == std::ifstream::traits_type::eof())
				break ;
			cgi_html << std::endl;

		}
		RequestMap.erase ("File");  
		RequestMap["File"] = "cgi.html";
		RequestMap.erase ("Extension");  
		RequestMap["Extension"] = "text/html";
		htmlTemplate.close();
		ToReplaceFile.close();
		cgi_html.close();
	}
	return(200);

}
