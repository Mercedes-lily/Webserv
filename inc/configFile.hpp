#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include "webserv.hpp"

class configFile {
private:
    std::string		_path;
    size_t			_size;

public:
    configFile();
    configFile(std::string const path);
    ~configFile();

    static int getTypePath(std::string const path);
    static int checkFile(std::string const path, int mode);
    std::string	readFile(std::string path);
    static int isFileExistAndReadable(std::string const path, std::string const index);

    std::string getPath();
    int getSize();
};

#endif