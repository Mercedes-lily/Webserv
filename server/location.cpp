#include "location.hpp"

location::location()
{
    this->_path = "";
    this->_root = "";
    this->_isAutoIndex = false;
    this->_index = "";
    this->_return = "";
    this->_alias = "";
    this->_client_max_body_size = MAX_SIZE;
    this->_allowed_methods.reserve(5);
    this->_allowed_methods.push_back(1);
    this->_allowed_methods.push_back(0);
    this->_allowed_methods.push_back(0);
    this->_allowed_methods.push_back(0);
    this->_allowed_methods.push_back(0);
}

location::location(const location& other)
{
    this->_path = other._path;
    this->_root = other._root;
    this->_isAutoIndex = other._isAutoIndex;
    this->_index = other._index;
    this->_cgi_path = other._cgi_path;
    this->_cgi_ext = other._cgi_ext;
    this->_return = other._return;
    this->_alias = other._alias;
    this->_allowed_methods = other._allowed_methods;
    this->_ext_path = other._ext_path;
    this->_client_max_body_size = other._client_max_body_size;
}

location& location::operator=(const location& rhs)
{
    if (this != &rhs)
    {
        this->_path = rhs._path;
        this->_root = rhs._root;
        this->_isAutoIndex = rhs._isAutoIndex;
        this->_index = rhs._index;
        this->_cgi_path = rhs._cgi_path;
        this->_cgi_ext = rhs._cgi_ext;
        this->_return = rhs._return;
        this->_alias = rhs._alias;
        this->_allowed_methods = rhs._allowed_methods;
        this->_ext_path = rhs._ext_path;
        this->_client_max_body_size = rhs._client_max_body_size;
    }
    return (*this);
}

location::~location() { }

//Setteux
void location::setPath(std::string str)
{
    this->_path = str;
}

void location::setRootLocation(std::string str)
{
    if (configFile::getTypePath(str) != 2)
        throw serverConfig::errorException("root of location");
    this->_root = str;
}

void location::setMethods(std::vector<std::string> methods)
{
    for (size_t i = 0; i < 5; i++)
    {
        this->_allowed_methods[i] = 0;
    }

    for (size_t i = 0; i < methods.size(); i++)
    {
        if (methods[i] == "GET")
            this->_allowed_methods[0] = 1;
        else if (methods[i] == "POST")
            this->_allowed_methods[1] = 1;
        else if (methods[i] == "DELETE")
            this->_allowed_methods[2] = 1;
        else if (methods[i] == "PUT")
            this->_allowed_methods[3] = 1;
        else if (methods[i] == "HEAD")
            this->_allowed_methods[4] = 1;
        else
            throw serverConfig::errorException("\033[31mAllow method not supported: " + methods[i] + "\033[0m");
    }
}

void location::setAutoindex(std::string str)
{
    if (str == "on" || str == "off")
        this->_isAutoIndex = (str == "on");
    else
        throw serverConfig::errorException("Wrong autoindex");
}

void location::setIndexLocation(std::string str)
{
    this->_index = str;
}

void location::setReturn(std::string str)
{
    this->_return = str;
}

void location::setAlias(std::string str)
{
    this->_alias = str;
}

void location::setCgiPath(std::vector<std::string> path)
{
    this->_cgi_path = path;
}

void location::setCgiExtension(std::vector<std::string> extension)
{
    this->_cgi_ext = extension;
}

void location::setMaxBodySize(std::string str)
{
    unsigned long body_size = 0;

    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] < '0' || str[i] > '9')
            throw serverConfig::errorException("Wrong syntax: client_max_body_size");
    }
    if (!ft_stoi(str))
        throw serverConfig::errorException("Wrong syntax: client_max_body_size");
    body_size = ft_stoi(str);
    this->_client_max_body_size = body_size;
}

void location::setMaxBodySize(unsigned long str)
{
    this->_client_max_body_size = str;
}

//Getteux
const std::string& location::getPath() const
{
    return (this->_path);
}

const std::string& location::getRootLocation() const
{
    return (this->_root);
}

const std::string& location::getIndexLocation() const
{
    return (this->_index);
}

const std::vector<int>& location::getMethods() const
{
    return (this->_allowed_methods);
}

const std::vector<std::string>& location::getCgiPath() const
{
    return (this->_cgi_path);
}

const std::vector<std::string>& location::getCgiExtension() const
{
    return (this->_cgi_ext);
}

const bool& location::getAutoindex() const
{
    return (this->_isAutoIndex);
}

const std::string& location::getReturn() const
{
    return (this->_return);
}

const std::string& location::getAlias() const
{
    return (this->_alias);
}

const std::map<std::string, std::string>& location::getExtensionPath() const
{
    return (this->_ext_path);
}

const unsigned long& location::getMaxBodySize() const
{
    return (this->_client_max_body_size);
}


std::string location::getPrintMethods() const
{
    std::string res;
    if (_allowed_methods[4])
        res.insert(0, "HEAD");
    if (_allowed_methods[3])
    {
        if (!res.empty())
            res.insert(0, ", ");
        res.insert(0, "PUT");
    }
    if (_allowed_methods[2])
    {
        if (!res.empty())
            res.insert(0, ", ");
        res.insert(0, "DELETE");
    }
    if (_allowed_methods[1])
    {
        if (!res.empty())
            res.insert(0, ", ");
        res.insert(0, "POST");
    }
    if (_allowed_methods[0])
    {
        if (!res.empty())
            res.insert(0, ", ");
        res.insert(0, "GET");
    }
    return (res);
}