#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream> // file I/O
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdint.h>
#include <set>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h> //opendir
#include <sys/stat.h> //stat

#include "location.hpp"
# include "configParser.hpp"
# include "configFile.hpp"
# include "serverConfig.hpp"
# include "location.hpp"

#define RESET "\033[0m"
#define HERE "\U00002757"
#define BLACK "\033[30m"  /* Black */
#define RED "\033[31m"    /* Red */
#define GREEN "\033[32m"  /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define BLUE "\033[34m"   /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m"   /* Cyan */
#define WHITE "\033[37m"  /* White */
#define BOLDBLACK "\033[1m\033[30m"  /* Bold Black */
#define BOLDRED "\033[1m\033[31m"    /* Bold Red */



#define MAX_SIZE 9999999

typedef struct	s_listen {
    unsigned int	host;
    int             port;
}				t_listen;

//utils.cpp
int ft_stoi(std::string str);
unsigned int fromHexToDec(const std::string& nb);
std::string statusCodeString(short statusCode);


#endif