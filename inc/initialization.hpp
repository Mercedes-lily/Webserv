#ifndef INITIALIZATION_HPP
# define INITIALIZATION_HPP

#include "serverConfig.hpp"


#include "errorCode.hpp"
#include "serverConfig.hpp"
#include <cstdlib>

using std::string;
using std::cout;
using std::endl;

int closefile(const string &str, int file);
int closefile(const string &str, int file, int file2);
int	nblock_connection(int fd, int reuse, int type);
int serverInitialization(serverConfig &serv);
int bindNlisten(serverConfig &serv);
int pollfdInitialization(serverConfig &serv);
int initialization(serverConfig &serv, int reuse);

# endif