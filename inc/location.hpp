#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webserv.hpp"

class location
{
private:
    std::string					_path;
    std::string					_root;
    bool						_isAutoIndex;
    std::string					_index;
    std::vector<int>			_allowed_methods;
    std::string					_return;
    std::string					_alias;
    std::vector<std::string>	_cgi_path;
    std::vector<std::string>	_cgi_ext;
    unsigned long				_client_max_body_size;

public:
    std::map<std::string, std::string> _ext_path;

    location();
    location(const location& other);
    location& operator=(const location& rhs);
    ~location();

    //Setter
    void setPath(std::string str);
    void setRootLocation(std::string str);
    void setMethods(std::vector<std::string> methods);
    void setAutoindex(std::string str);
    void setIndexLocation(std::string str);
    void setReturn(std::string str);
    void setAlias(std::string str);
    void setCgiPath(std::vector<std::string> path);
    void setCgiExtension(std::vector<std::string> extension);
    void setMaxBodySize(std::string str);
    void setMaxBodySize(unsigned long str);

    //Getter
    const std::string& getPath() const;
    const std::string& getRootLocation() const;
    const std::vector<int>& getMethods() const;
    const bool& getAutoindex() const;
    const std::string& getIndexLocation() const;
    const std::string& getReturn() const;
    const std::string& getAlias() const;
    const std::vector<std::string>& getCgiPath() const;
    const std::vector<std::string>& getCgiExtension() const;
    const std::map<std::string, std::string>& getExtensionPath() const;
    const unsigned long& getMaxBodySize() const;

    std::string getPrintMethods() const;

};

#endif