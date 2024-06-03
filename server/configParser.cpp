#include "configParser.hpp"

configParser::configParser()
{
    this->_nb_server = 0;
}

configParser::~configParser() { }

int configParser::initializeServerConfigurations(const std::string& config_file)
{
    std::string		strPtr;
    configFile		file(config_file);

    if (file.getTypePath(file.getPath()) != 1)
        throw errorException("\033[31mProvided path does not point to a file\033[0m");
    if (file.checkFile(file.getPath(), 4) == -1)
        throw errorException("\033[31mFile is not accessible or does not exist\033[0m");
    strPtr = file.readFile(config_file);
    if (strPtr.empty())
        throw errorException("\033[31mConfiguration file is empty\033[0m");
    removeHashtagComments(strPtr);
    trimLeadingTrailingWhitespace(strPtr);
    extractServerConfigurations(strPtr);
    if (this->_server_conf.size() != this->_nb_server)
        throw errorException("\033[31mMismatch between expected and actual number of server configurations\033[0m");
    for (size_t i = 0; i < this->_nb_server; i++)
    {
        serverConfig server;
        parseServerConfiguration(this->_server_conf[i], server);
        this->_servers.push_back(server);
    }
    if (this->_nb_server > 1)
        validateServerConfigurations();
    return (0);
}

void configParser::removeHashtagComments(std::string& strPtr)
{
    size_t pos = strPtr.find('#');
    while (pos != std::string::npos)
    {
        strPtr.erase(pos, strPtr.find('\n', pos) - pos);
        pos = strPtr.find('#');
    }
}

void configParser::trimLeadingTrailingWhitespace(std::string& strPtr)
{
    size_t start = strPtr.find_first_not_of(" \t\n\r\f\v");
    size_t end = strPtr.find_last_not_of(" \t\n\r\f\v");

    if (start != std::string::npos)
        strPtr = strPtr.substr(start, end - start + 1);
    else
        strPtr = "";
}

void configParser::extractServerConfigurations(std::string& strPtr)
{
    size_t start = 0;
    size_t end = 1;

    if (strPtr.find("server", 0) == std::string::npos)
        throw errorException("\033[31mServer did not find\033[0m");
    while (start != end && start < strPtr.length())
    {
        start = findServerBlockStart(start, strPtr);
        end = findServerBlockEnd(start, strPtr);
        if (start == end)
            throw errorException("\033[31mProblem with scope\033[0m");
        this->_server_conf.push_back(strPtr.substr(start, end - start + 1));
        this->_nb_server++;
        start = end + 1;
    }
}

size_t configParser::findServerBlockStart(size_t start, std::string& strPtr)
{
    size_t i;

    for (i = start; strPtr[i]; i++)
    {
        if (strPtr[i] == 's')
            break;
        if (!isspace(strPtr[i]))
            throw  errorException("\033[31mUnexpected character outside of server scope{}\033[0m");
    }
    if (!strPtr[i])
        return (start);
    if (strPtr.compare(i, 6, "server") != 0)
        throw errorException("\033[31mUnexpected character outside of server scope{}\033[0m");
    i += 6;
    while (strPtr[i] && isspace(strPtr[i]))
        i++;
    if (strPtr[i] == '{')
        return (i);
    else
        throw  errorException("\033[31mUnexpected character outside of server scope{}\033[0m");
}

size_t configParser::findServerBlockEnd(size_t start, std::string& strPtr)
{
    size_t	i;
    size_t	scope;

    scope = 0;
    for (i = start + 1; strPtr[i]; i++)
    {
        if (strPtr[i] == '{')
            scope++;
        if (strPtr[i] == '}')
        {
            if (!scope)
                return (i);
            scope--;
        }
    }
    return (start);
}

std::vector<std::string> splitStringByDelimiter(std::string line, std::string sep)
{
    std::vector<std::string>	str;
    std::string::size_type		start, end;

    start = end = 0;
    while (1)
    {
        end = line.find_first_of(sep, start);
        if (end == std::string::npos)
            break;
        std::string tmp = line.substr(start, end - start);
        str.push_back(tmp);
        start = line.find_first_not_of(sep, end);
        if (start == std::string::npos)
            break;
    }
    return (str);
}

void configParser::parseServerConfiguration(std::string& config, serverConfig& server)
{
    std::vector<std::string>	param;
    std::vector<std::string>	error_codes;
    int		flag_loc = 1;
    bool	flag_autoindex = false;
    bool	flag_max_size = false;
    std::string errorMessage = "";

    param = splitStringByDelimiter(config += ' ', std::string(" \n\t"));
    if (param.size() < 3)
        errorMessage += "Failed server validation. ";

    for (size_t i = 0; i < param.size(); i++)
    {
        if (param[i] == "listen" && (i + 1) < param.size() && flag_loc)
        {
            if (server.getPort())
                errorMessage += "Port is duplicated. ";
            server.setPort(param[++i]);
        }
        else if (param[i] == "location" && (i + 1) < param.size())
        {
            std::string	path;
            i++;
            if (param[i] == "{" || param[i] == "}")
                errorMessage += "Wrong character in server scope{}. ";
            path = param[i];
            std::vector<std::string> codes;
            if (param[++i] != "{")
                errorMessage += "Wrong character in server scope{}. ";
            i++;
            while (i < param.size() && param[i] != "}")
                codes.push_back(param[i++]);
            server.setLocation(path, codes);
            if (i < param.size() && param[i] != "}")
                errorMessage += "Wrong character in server scope{}. ";
            flag_loc = 0;
        }
        else if (param[i] == "host" && (i + 1) < param.size() && flag_loc)
        {
            if (server.getHost())
                errorMessage += "Host is duplicated. ";
            server.setHost(param[++i]);
        }
        else if (param[i] == "root" && (i + 1) < param.size() && flag_loc)
        {
            if (!server.getRoot().empty())
                errorMessage += "Root is duplicated. ";
            server.setRoot(param[++i]);
        }
        else if (param[i] == "error_page" && (i + 1) < param.size() && flag_loc)
        {
            while (++i < param.size())
            {
                error_codes.push_back(param[i]);
                if (param[i].find(';') != std::string::npos)
                    break;
                if (i + 1 >= param.size())
                    errorMessage += "Wrong character out of server scope{}. ";
            }
        }
        else if (param[i] == "client_max_body_size" && (i + 1) < param.size() && flag_loc)
        {
            if (flag_max_size)
                errorMessage += "Client_max_body_size is duplicated. ";
            server.setClientMaxBodySize(param[++i]);
            flag_max_size = true;
        }
        else if (param[i] == "server_name" && (i + 1) < param.size() && flag_loc)
        {
            if (!server.getServerName().empty())
                errorMessage += "Server_name is duplicated. ";
            server.setServerName(param[++i]);
        }
        else if (param[i] == "index" && (i + 1) < param.size() && flag_loc)
        {
            if (!server.getIndex().empty())
                errorMessage += "Index is duplicated. ";
            server.setIndex(param[++i]);
        }
        else if (param[i] == "autoindex" && (i + 1) < param.size() && flag_loc)
        {
            if (flag_autoindex)
                errorMessage += "Autoindex of server is duplicated. ";
            server.setAutoindex(param[++i]);
            flag_autoindex = true;
        }
        else if (param[i] != "}" && param[i] != "{")
        {
            if (!flag_loc)
                errorMessage += "Parameters after location. ";
            else
                errorMessage += "Unsupported directive. ";
        }
    }
    if (server.getRoot().empty())
        server.setRoot("/;");
    if (server.getHost() == 0)
        server.setHost("localhost;");
    if (server.getIndex().empty())
        server.setIndex("index.html;");
    if (configFile::isFileExistAndReadable(server.getRoot(), server.getIndex()))
        errorMessage += "Index from config file not found or unreadable. ";
    if (server.checkLocations())
        errorMessage += "Location is duplicated. ";
    if (!server.getPort())
        errorMessage += "Port not found. ";
    server.setErrorPages(error_codes);
    if (!server.isValidErrorPages())
        errorMessage += "Incorrect path for error page or number of error. ";

    if (!errorMessage.empty())
        throw errorException("\033[31m" + errorMessage + "\033[0m");
}

int	configParser::compareSubstringsAtPosition(std::string str1, std::string str2, size_t pos)
{
    size_t	i;

    i = 0;
    while (pos < str1.length() && i < str2.length() && str1[pos] == str2[i])
    {
        pos++;
        i++;
    }
    if (i == str2.length() && pos <= str1.length() && (str1.length() == pos || isspace(str1[pos])))
        return (0);
    return (1);
}

void configParser::validateServerConfigurations()
{
    std::vector<serverConfig>::iterator it1;
    std::vector<serverConfig>::iterator it2;

    for (it1 = this->_servers.begin(); it1 != this->_servers.end() - 1; it1++)
    {
        for (it2 = it1 + 1; it2 != this->_servers.end(); it2++)
        {
            if (it1->getPort() == it2->getPort() && it1->getHost() == it2->getHost() && it1->getServerName() == it2->getServerName())
                throw errorException("\033[31mFailed server validation\033[0m");
        }
    }
}

std::vector<serverConfig>	&configParser::getServers()
{
    return (this->_servers);
}

int configParser::print()
{
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        serverConfig& server = _servers[i];
        std::cout << BLUE << "Server #" << i + 1 << std::endl;
        std::cout << "Server name: " << server.getServerName() << std::endl;
        std::cout << "Host: " << server.getHost() << std::endl;
        std::cout << "Root: " << server.getRoot() << std::endl;
        std::cout << "Index: " << server.getIndex() << std::endl;
        std::cout << "Port: " << server.getPort() << std::endl;
        std::cout << "Max BSize: " << server.getClientMaxBodySize() << std::endl;
        const std::map<short, std::string>& errorPages = server.getErrorPages();
        std::cout << "Error pages: " << errorPages.size() << RESET << std::endl;
        for (std::map<short, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
        {
            std::cout << it->first << " - " << it->second << std::endl;
        }
        const std::vector<location>& locations = server.getLocations();
        std::cout << "Locations: " << locations.size() << std::endl;
        for (std::vector<location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
        {
            std::cout << "name location: " << it->getPath() << std::endl;
            std::cout << "methods: " << it->getPrintMethods() << std::endl;
            std::cout << "index: " << it->getIndexLocation() << std::endl;
            if (it->getCgiPath().empty())
            {
                std::cout << "root: " << it->getRootLocation() << std::endl;
                if (!it->getReturn().empty())
                    std::cout << "return: " << it->getReturn() << std::endl;
                if (!it->getAlias().empty())
                    std::cout << "alias: " << it->getAlias() << std::endl;
            }
            else
            {
                std::cout << "cgi root: " << it->getRootLocation() << std::endl;
                std::cout << "sgi_path: " << it->getCgiPath().size() << std::endl;
                std::cout << "sgi_ext: " << it->getCgiExtension().size() << std::endl;
            }
        }
    }
    return 0;
}

size_t configParser::get_nb_server()
{
    return(this->_nb_server);
}