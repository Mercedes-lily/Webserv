#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "webserv.hpp"

class serverConfig;

class configParser {
private:
    std::vector<serverConfig>	_servers;
    std::vector<std::string>	_server_conf;
    size_t						_nb_server;

public:

    configParser();
    ~configParser();

    int initializeServerConfigurations(const std::string& config_file);

    void extractServerConfigurations(std::string& ptr);
    void removeHashtagComments(std::string& ptr);
    void trimLeadingTrailingWhitespace(std::string& ptr);
    size_t findServerBlockStart(size_t start, std::string& ptr);
    size_t findServerBlockEnd(size_t start, std::string& ptr);
    void parseServerConfiguration(std::string& config, serverConfig& server);
    void validateServerConfigurations();
    std::vector<serverConfig> &getServers();
    int	compareSubstringsAtPosition(std::string str1, std::string str2, size_t pos);

    int print();
    size_t get_nb_server();

public:
    class errorException : public std::exception
    {
    private:
        std::string _msg;
    public:
        errorException(std::string msg) throw()
        {
            _msg = "\033[31mErreur configParser: \033[0m" + msg;
        }
        virtual const char* what() const throw()
        {
            return (_msg.c_str());
        }
        virtual ~errorException() throw() {}
    };
};

#endif