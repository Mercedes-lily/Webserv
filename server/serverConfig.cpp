#include "serverConfig.hpp"
#include "configFile.hpp"

serverConfig::serverConfig()
{
    this->_port = 0;
    this->_host = 0;
    this->_server_name = "";
    this->_root = "";
    this->_client_max_body_size = MAX_SIZE;
    this->_index = "";
    this->_listen_fd = 0;
    this->_isAutoIndex = false;
    this->initErrorPages();
	//provient de la classe faite par Sébastien et valérie
	this->_backlog = 3;
    this->_addrlen = sizeof(struct sockaddr_in);
    this->_socket_client = 0;
    this->_maxfds = 1;
    this->_numfds = 1;
    this->_nfds = 1 ;
}

serverConfig::~serverConfig()
{
}

serverConfig::serverConfig(const serverConfig& copy)
{
    if (this != &copy)
    {
        this->_server_name = copy._server_name;
        this->_root = copy._root;
        this->_host = copy._host;
        this->_port = copy._port;
        this->_client_max_body_size = copy._client_max_body_size;
        this->_index = copy._index;
        this->_error_pages = copy._error_pages;
        this->_locations = copy._locations;
        this->_listen_fd = copy._listen_fd;
        this->_isAutoIndex = copy._isAutoIndex;
        this->_server_address = copy._server_address;
		//provient de la classe faite par Sébastien et valérie
		this->_server_fd = copy.getServer_fd();
		this->_socket_client = copy.getSocket_client();
		this->_maxfds = copy.getMaxfds();
		this->_numfds = copy.getNumfds();
		this->_nfds = copy.getNfds();
        this->_pollfds = copy.getPollfds();

    }
    return;
}

serverConfig& serverConfig::operator=(const serverConfig& rhs)
{
    if (this != &rhs)
    {
        this->_server_name = rhs._server_name;
        this->_root = rhs._root;
        this->_port = rhs._port;
        this->_host = rhs._host;
        this->_client_max_body_size = rhs._client_max_body_size;
        this->_index = rhs._index;
        this->_error_pages = rhs._error_pages;
        this->_locations = rhs._locations;
        this->_listen_fd = rhs._listen_fd;
        this->_isAutoIndex = rhs._isAutoIndex;
        this->_server_address = rhs._server_address;
		//provient de la classe faite par Sébastien et valérie
		this->_server_fd = rhs.getServer_fd();
		this->_socket_client = rhs.getSocket_client();
		this->_maxfds = rhs.getMaxfds();
		this->_numfds = rhs.getNumfds();
		this->_nfds = rhs.getNfds();
        this->_pollfds = rhs.getPollfds();
    }
    return (*this);
}

/* init error page by default */
void serverConfig::initErrorPages(void)
{
    _error_pages[301] = "";
    _error_pages[302] = "";
    _error_pages[400] = "error_pages/400.html";
    _error_pages[401] = "error_pages/401.html";
    _error_pages[402] = "";
    _error_pages[403] = "error_pages/403.html";
    _error_pages[404] = "error_pages/404.html";
    _error_pages[405] = "error_pages/405.html";;
    _error_pages[406] = "";
	_error_pages[408] = "error_pages/408.html";
	_error_pages[409] = "error_pages/409.html";
	_error_pages[411] = "error_pages/411.html";
	_error_pages[415] = "error_pages/415.html";
    _error_pages[500] = "error_pages/500.html";
    _error_pages[501] = "error_pages/501.html";
    _error_pages[502] = "";
    _error_pages[503] = "";
    _error_pages[505] = "error_pages/505.html";
}

//Setteux
void serverConfig::setServerName(std::string server_name)
{
    checkToken(server_name);
    this->_server_name = server_name;
}

void serverConfig::setHost(std::string strPtr)
{
    checkToken(strPtr);
    if (strPtr == "localhost")
        strPtr = "127.0.0.1";
    if (!isValidHost(strPtr))
        throw errorException("\033m[31mserverConfig : host\033[0m");
    this->_host = inet_addr(strPtr.data());
}

void	serverConfig::setAddress()
{
	this->_server_address.sin_family = AF_INET;
	this->_server_address.sin_port = htons(this->_port);
	this->_server_address.sin_addr.s_addr = INADDR_ANY;
}

void serverConfig::setRoot(std::string root)
{
    checkToken(root);
    if (configFile::getTypePath(root) == 2)
    {
        this->_root = root;
        return;
    }
    char dir[1024];
    getcwd(dir, 1024);
    std::string full_root = dir + root;
    if (configFile::getTypePath(full_root) != 2)
        throw errorException("\033m[31mserverConfig : root\033[0m");
    this->_root = full_root;
}

void serverConfig::setPort(std::string strPtr)
{
    unsigned int port;

    port = 0;
    checkToken(strPtr);
    for (size_t i = 0; i < strPtr.length(); i++)
    {
        if (!std::isdigit(strPtr[i]))
            throw errorException("\033m[31mserverConfig : port\033[0m");
    }
    port = ft_stoi((strPtr));
    if (port < 1 || port > 65635)
        throw errorException("\033m[31mserverConfig : port\033[0m");
    this->_port = (uint16_t)port;
}

void serverConfig::setClientMaxBodySize(std::string strPtr)
{
    unsigned long body_size;

    body_size = 0;
    checkToken(strPtr);
    for (size_t i = 0; i < strPtr.length(); i++)
    {
        if (strPtr[i] < '0' || strPtr[i] > '9')
            throw errorException("\033m[31mserverConfig : client_body_size\033[0m");
    }
    if (!ft_stoi(strPtr))
        throw errorException("\033m[31mserverConfig : client_body_size\033[0m");
    body_size = ft_stoi(strPtr);
    this->_client_max_body_size = body_size;
}

void serverConfig::setIndex(std::string index)
{
    checkToken(index);
    this->_index = index;
}

void serverConfig::setAutoindex(std::string autoindex)
{
    checkToken(autoindex);
    if (autoindex != "on" && autoindex != "off")
        throw errorException("\033m[31mserverConfig : auto index\033[0m");
    if (autoindex == "on")
        this->_isAutoIndex = true;
}

void serverConfig::setErrorPages(std::vector<std::string>& strPtr)
{
    if (strPtr.empty())
        return;
    if (strPtr.size() % 2 != 0)
        throw errorException("\033m[31mserverConfig : error page invalid\033[0m");
    for (size_t i = 0; i < strPtr.size() - 1; i++)
    {
        for (size_t j = 0; j < strPtr[i].size(); j++) {
            if (!std::isdigit(strPtr[i][j]))
                throw errorException("\033m[31mserverConfig : error page invalid\033[0m");
        }
        if (strPtr[i].size() != 3)
            throw errorException("\033m[31mserverConfig : error code invalid\033[0m");
        short code_error = ft_stoi(strPtr[i]);
        if (statusCodeString(code_error) == "Undefined" || code_error < 400)
            throw errorException("Incorrect error code: " + strPtr[i]);
        i++;
        std::string path = strPtr[i];
        checkToken(path);
        if (configFile::getTypePath(path) != 2)
        {
            if (configFile::checkFile(this->_root + path, 0) == -1 || configFile::checkFile(this->_root + path, 4) == -1 || configFile::getTypePath(this->_root + path) != 1)
                throw errorException("\033m[31mserverConfig : error page pas accessible\033[0m");
        }
        std::map<short, std::string>::iterator it = this->_error_pages.find(code_error);
        if (it != _error_pages.end())
            this->_error_pages[code_error] = path;
        else
            this->_error_pages.insert(std::make_pair(code_error, path));
    }
}

void serverConfig::setLocation(std::string path, std::vector<std::string> strPtr)
{
    location new_location;
    std::vector<std::string> methods;
    bool flag_methods = false;
    bool flag_autoindex = false;
    bool flag_max_size = false;
    int valid;

    new_location.setPath(path);
    for (size_t i = 0; i < strPtr.size(); i++)
    {
        if (strPtr[i] == "root" && (i + 1) < strPtr.size())
        {
            if (!new_location.getRootLocation().empty())
                throw errorException("\033m[31mserverConfig : Duplicate root location\033[0m");
            checkToken(strPtr[++i]);
            if (configFile::getTypePath(strPtr[i]) == 2)
                new_location.setRootLocation(strPtr[i]);
            else
                new_location.setRootLocation(this->_root + strPtr[i]);
        }
        else if ((strPtr[i] == "allow_methods" || strPtr[i] == "methods") && (i + 1) < strPtr.size())
        {
            if (flag_methods)
                throw errorException("\033m[31mserverConfig : Duplicate allow_methods\033[0m");
            std::vector<std::string> methods;
            while (++i < strPtr.size())
            {
                if (strPtr[i].find(";") != std::string::npos)
                {
                    checkToken(strPtr[i]);
                    methods.push_back(strPtr[i]);
                    break;
                }
                else
                {
                    methods.push_back(strPtr[i]);
                    if (i + 1 >= strPtr.size())
                        throw errorException("\033m[31mserverConfig : Token is invalid\033[0m");
                }
            }
            new_location.setMethods(methods);
            flag_methods = true;
        }
        else if (strPtr[i] == "autoindex" && (i + 1) < strPtr.size())
        {
            if (path == "/cgi-bin")
                throw errorException("\033m[31mserverConfig : Autoindex not allowed for CGI\033[0m");
            if (flag_autoindex)
                throw errorException("\033m[31mserverConfig : Duplicate autoindex\033[0m");
            checkToken(strPtr[++i]);
            new_location.setAutoindex(strPtr[i]);
            flag_autoindex = true;
        }
        else if (strPtr[i] == "index" && (i + 1) < strPtr.size())
        {
            if (!new_location.getIndexLocation().empty())
                throw errorException("\033m[31mserverConfig : Duplicate autoindex\033[0m");
            checkToken(strPtr[++i]);
            new_location.setIndexLocation(strPtr[i]);
        }
        else if (strPtr[i] == "return" && (i + 1) < strPtr.size())
        {
            if (path == "/cgi-bin")
                throw errorException("\033m[31mReturn not allowed for CGI\033[0m");
            if (!new_location.getReturn().empty())
                throw errorException("\033m[31mDuplicate return\033[0m");
            checkToken(strPtr[++i]);
            new_location.setReturn(strPtr[i]);
        }
        else if (strPtr[i] == "alias" && (i + 1) < strPtr.size())
        {
            if (path == "/cgi-bin")
                throw errorException("\033m[31mAlias not allowed for CGI\033[0m");
            if (!new_location.getAlias().empty())
                throw errorException("\033m[31mDuplicate alias\033[0m");
            checkToken(strPtr[++i]);
            new_location.setAlias(strPtr[i]);
        }
        else if (strPtr[i] == "cgi_ext" && (i + 1) < strPtr.size())
        {
            std::vector<std::string> extension;
            while (++i < strPtr.size())
            {
                if (strPtr[i].find(";") != std::string::npos)
                {
                    checkToken(strPtr[i]);
                    extension.push_back(strPtr[i]);
                    break;
                }
                else
                {
                    extension.push_back(strPtr[i]);
                    if (i + 1 >= strPtr.size())
                        throw errorException("\03m[31mserverConfig : Token is invalid\033[0m");
                }
            }
            new_location.setCgiExtension(extension);
        }
        else if (strPtr[i] == "cgi_path" && (i + 1) < strPtr.size())
        {
            std::vector<std::string> path;
            while (++i < strPtr.size())
            {
                if (strPtr[i].find(";") != std::string::npos)
                {
                    checkToken(strPtr[i]);
                    path.push_back(strPtr[i]);
                    break;
                }
                else
                {
                    path.push_back(strPtr[i]);
                    if (i + 1 >= strPtr.size())
                        throw errorException("\033m[31mserverConfig : Token is invalid\033[0m");
                }
                if (strPtr[i].find("/php") == std::string::npos)
                    throw errorException("\033m[31mserverConfig : cgi_path is invalid\033[0m");
            }
            new_location.setCgiPath(path);
        }
        else if (strPtr[i] == "client_max_body_size" && (i + 1) < strPtr.size())
        {
            if (flag_max_size)
                throw errorException("\033m[31mserverConfig : Maxbody_size of location is duplicated\033[0m");
            checkToken(strPtr[++i]);
            new_location.setMaxBodySize(strPtr[i]);
            flag_max_size = true;
        }
        else if (i < strPtr.size())
            throw errorException("\033m[31mserverConfig : Parametr in a location is invalid\033[0m");
    }
    if (new_location.getPath() != "/cgi-bin" && new_location.getIndexLocation().empty())
        new_location.setIndexLocation(this->_index);
    if (!flag_max_size)
        new_location.setMaxBodySize(this->_client_max_body_size);
    valid = locationChecker(new_location);
    if (valid == 1 || valid == 2 || valid == 3 || valid == 4)
        throw errorException("\033m[31mserverConfig : ERROR\033[0m");
    this->_locations.push_back(new_location);
}

void	serverConfig::setFd(int fd)
{
    this->_listen_fd = fd;
}

void	serverConfig::setServer_fd(int serv)
{
	this->_server_fd = serv;
}

void	serverConfig::setSocket_client(int serv)
{
	this->_socket_client = serv;
}

void	serverConfig::setMaxfds(int max)
{
	this->_maxfds = max;
}

void	serverConfig::setNumfds(int n)
{
	this->_numfds = n;
}

void	serverConfig::setNfds(int n)
{
	this->_nfds = n;
}

void	serverConfig::setFile(string &str)
{
	this->_file = str;
}

std::fstream&	serverConfig::getFile_fd()
{
	return(this->_file_fd);
}

bool serverConfig::isValidHost(std::string host) const
{
    struct sockaddr_in sockaddr;
    return (inet_pton(AF_INET, host.c_str(), &(sockaddr.sin_addr)) ? true : false);
}

bool serverConfig::isValidErrorPages()
{
    std::map<short, std::string>::const_iterator it;
    for (it = this->_error_pages.begin(); it != this->_error_pages.end(); it++)
    {
        if (it->first < 100 || it->first > 599)
            return (false);
        if (configFile::checkFile(getRoot() + it->second, 0) < 0 || configFile::checkFile(getRoot() + it->second, 4) < 0)
            return (false);
    }
    return (true);
}

int serverConfig::locationChecker(location& location) const
{
    if (location.getPath() == "/cgi-bin")
    {
        if (location.getCgiPath().empty() || location.getCgiExtension().empty() || location.getIndexLocation().empty())
            return (1);


        if (configFile::checkFile(location.getIndexLocation(), 4) < 0)
        {
            std::string path = location.getRootLocation() + location.getPath() + "/" + location.getIndexLocation();
            if (configFile::getTypePath(path) != 1)
            {
                std::string root = getcwd(NULL, 0);
                location.setRootLocation(root);
                path = root + location.getPath() + "/" + location.getIndexLocation();
            }
            if (path.empty() || configFile::getTypePath(path) != 1 || configFile::checkFile(path, 4) < 0)
                return (1);
        }
        if (location.getCgiPath().size() != location.getCgiExtension().size())
            return (1);
        std::vector<std::string>::const_iterator it;
        for (it = location.getCgiPath().begin(); it != location.getCgiPath().end(); ++it)
        {
            if (configFile::getTypePath(*it) < 0)
                return (1);
        }
        std::vector<std::string>::const_iterator it_path;
        for (it = location.getCgiExtension().begin(); it != location.getCgiExtension().end(); ++it)
        {
            std::string tmp = *it;
            if (tmp != ".php" && tmp != "*.php")
                return (1);
            for (it_path = location.getCgiPath().begin(); it_path != location.getCgiPath().end(); ++it_path)
            {
                std::string tmp_path = *it_path;
                if (tmp == ".php" || tmp == "*.php")
                {
                    if (tmp_path.find("php") != std::string::npos)
                        location._ext_path.insert(std::make_pair(".php", tmp_path));
                }
            }
        }
        if (location.getCgiPath().size() != location.getExtensionPath().size())
            return (1);
    }
    else
    {
        if (location.getPath()[0] != '/')
            return (2);
        if (location.getRootLocation().empty()) {
            location.setRootLocation(this->_root);
        }
        if (configFile::isFileExistAndReadable(location.getRootLocation() + location.getPath() + "/", location.getIndexLocation()))
            return (5);
        if (!location.getReturn().empty())
        {
            if (configFile::isFileExistAndReadable(location.getRootLocation(), location.getReturn()))
                return (3);
        }
        if (!location.getAlias().empty())
        {
            if (configFile::isFileExistAndReadable(location.getRootLocation(), location.getAlias()))
                return (4);
        }
    }
    return (0);
}

/* Getteux */
const std::string& serverConfig::getServerName()
{
    return (this->_server_name);
}

const std::string& serverConfig::getRoot()
{
    return (this->_root);
}

const bool& serverConfig::getAutoindex()
{
    return (this->_isAutoIndex);
}

const in_addr_t& serverConfig::getHost()
{
    return (this->_host);
}

const uint16_t& serverConfig::getPort()
{
    return (this->_port);
}

const size_t& serverConfig::getClientMaxBodySize()
{
    return (this->_client_max_body_size);
}

const std::vector<location>& serverConfig::getLocations()
{
    return (this->_locations);
}

const std::map<short, std::string>& serverConfig::getErrorPages()
{
    return (this->_error_pages);
}

const std::string& serverConfig::getIndex()
{
    return (this->_index);
}

int   	serverConfig::getFd()
{
    return (this->_listen_fd);
}

const struct sockaddr_in	&serverConfig::getAddress() const
{
	return(this->_server_address);
}

struct sockaddr	*serverConfig::getSockaddr()
{
	struct sockaddr *newaddr = reinterpret_cast<struct sockaddr*>(&this->_server_address);
	return(newaddr);
}

int	serverConfig::getServer_fd() const
{
	return(this->_server_fd);
}

socklen_t	serverConfig::getAddrlen() const
{
	return(this->_addrlen);
}

int	serverConfig::getBacklog() const
{
	return(this->_backlog);
}

int	serverConfig::getSocket_client() const
{
	return(this->_socket_client);
}

int	serverConfig::getMaxfds() const
{
	return(this->_maxfds);
}

int	serverConfig::getNumfds() const
{
	return(this->_numfds);
}

int	serverConfig::getNfds() const
{
	return(this->_nfds);
}

string&	serverConfig::getFile()
{
	return(this->_file);
}

struct pollfd *serverConfig::getPollfds() const
{
    return(this->_pollfds);
}

void serverConfig::setPollfds(struct pollfd * p)
{
    this->_pollfds = p;
}

void serverConfig::checkToken(std::string& strPtr)
{
    size_t pos = strPtr.rfind(';');
    if (pos != strPtr.size() - 1)
        throw errorException("\033[34mInvalid Token!\033[0m");
    strPtr.erase(pos);
}

bool serverConfig::checkLocations() const
{
    if (this->_locations.size() < 2)
        return (false);
    std::vector<location>::const_iterator it1;
    std::vector<location>::const_iterator it2;
    for (it1 = this->_locations.begin(); it1 != this->_locations.end() - 1; it1++) {
        for (it2 = it1 + 1; it2 != this->_locations.end(); it2++) {
            if (it1->getPath() == it2->getPath())
                return (true);
        }
    }
    return (false);
}