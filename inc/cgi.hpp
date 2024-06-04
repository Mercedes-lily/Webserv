#ifndef CGI_HPP
# define CGI_HPP

#include "serverConfig.hpp"
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>

std::string find_replace(std::string readed, std::string s1, std::string s2);
int iscgi(std::string file, std::string extension);
int executecgi(std::map<string, string> &RequestMap);

#endif