#include "../inc/Request.hpp"
#include "../inc/initialization.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>

/*POST /resource HTTP/1.1
Host: api.example.com
Content-Type: application/json
Authorization: Bearer abc123token
Content-Length: 35

{
    "name": "example",
    "type": "test"
}
*/

int postMethode(std::map<string, string> &RequestMap, serverConfig &serv)
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
	std::string file_to_upload;
	std::cout << "File in post before stat " << serv.getRoot() << std::endl;
	if(RequestMap["Extension"] == "py" || RequestMap["Extension"] == "php" || RequestMap["Extension"] == "sh" )
		file_to_upload = "server/script" + RequestMap["File"];
	else
		file_to_upload = serv.getRoot() + "upload" + RequestMap["File"];
	if (stat(file_to_upload.c_str(), &sb) == 0)
	{
		std::cout << "The file already exists" << endl;
		return (1);
	}
	if (S_ISDIR(sb.st_mode))
	{
        cout << "Ends with /" << endl;
        return (409);
	}
    const size_t length = atol(RequestMap["Content-Length"].c_str());
    if (length <= 0)
    {
        std::cout << "Content-Length is 0 or less" << std::endl;
        return (411);
    }
    if (length > serv.getClientMaxBodySize())
    {
        std::cout << "Content-Length " << std::endl;
        return (413);
    }
    std::ofstream upload_html(file_to_upload);
    upload_html.write(RequestMap["Body"].c_str(), length);
	//TODO change response with the right values
	string text = "HTTP/1.1 ";
	text += "200 OK\r\nContent-Type: text/html\r\n\r\n";
	send(serv.getSocket_client(), text.c_str(), text.size(), 0);
	return(0);
}