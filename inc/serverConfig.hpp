#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "webserv.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <poll.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <sys/stat.h>

using std::string;
using std::exception;
using std::cout;
using std::endl;
using std::remove;

static std::string	serverParametrs[] = { "server_name", "listen", "root", "index", "allow_methods", "client_body_buffer_size" };

class location;

class serverConfig
{
private:
    uint16_t						_port;
    in_addr_t						_host;
    std::string						_server_name;
    std::string						_root;
    unsigned long					_client_max_body_size;
    std::string						_index;
    bool							_isAutoIndex;
    std::map<short, std::string>	_error_pages; //ajouter les pages faites par Sébastien
    std::vector<location> 			_locations;
    struct sockaddr_in 				_server_address;
    int     						_listen_fd;  // qu'est-ce que c'est
	//provient de la classe faite par Sébastien et valérie
	int						        _backlog;
	socklen_t					    _addrlen;
	int								_server_fd;
	int								_socket_client;
	int								_maxfds;
	int								_numfds;
	int								_nfds;
	string							_file;
	std::fstream					_file_fd;
    struct pollfd *                 _pollfds;

public:
    serverConfig();
    ~serverConfig();
    serverConfig(const serverConfig& other);
    serverConfig& operator=(const serverConfig& rhs);

    void initErrorPages(void);

	//Setter
	void setServerName(std::string server_name);
	void setHost(std::string str);
	void setRoot(std::string root);
	void setFd(int);
	void setPort(std::string str);
	void setClientMaxBodySize(std::string str);
	void setErrorPages(std::vector<std::string>& str);
	void setIndex(std::string index);
	void setLocation(std::string nameLocation, std::vector<std::string> str);
	void setAutoindex(std::string autoindex);
	//provient de la classe faite par Sébastien et valérie
	void	setAddress();
	void	setServer_fd(int serv);
	void	setSocket_client(int serv);
	void	setMaxfds(int max);
	void	setNumfds(int n);
	void	setNfds(int n);
	void	setFile(string &str);
    void	setPollfds(struct pollfd * p);

    //Getter
    const std::string& getServerName();
    const uint16_t& getPort();
    const in_addr_t& getHost();
    const size_t& getClientMaxBodySize();
    const std::vector<location>& getLocations();
    const std::string& getRoot();
    const std::map<short, std::string>& getErrorPages();
    const std::string& getIndex();
    const bool& getAutoindex();
    int     getFd();
	//provient de la classe faite par Sébastien et valérie
	const struct sockaddr_in	&getAddress() const;
	struct sockaddr	*getSockaddr();
	int	getBacklog() const;
	socklen_t	getAddrlen() const;
	int	getServer_fd() const;
	int	getSocket_client() const;
	int	getMaxfds() const;
	int	getNumfds() const;
	int	getNfds() const;
	string&	getFile();
	std::fstream&	getFile_fd();
	struct pollfd *getPollfds() const;

    static void checkToken(std::string& str);
    bool checkLocations() const;
    bool isValidHost(std::string host) const;
    bool isValidErrorPages();
    int locationChecker(location& location) const;

public:
    class errorException : public std::exception
    {
    private:
        std::string _msg;
    public:
        errorException(std::string msg) throw()
        {
            _msg = "\033[31mErreur serverConfig: \033[0m" + msg;
        }
        virtual const char* what() const throw()
        {
            return (_msg.c_str());
        }
        virtual ~errorException() throw() {}
    };

};

#endif