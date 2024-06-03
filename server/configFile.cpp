#include "configFile.hpp"

configFile::configFile() : _size(0) { }

configFile::configFile(std::string const path) : _path(path), _size(0) { }

configFile::~configFile() { }

int configFile::getTypePath(std::string const path)
{
    struct stat	buffer;
    int			result;

    result = stat(path.c_str(), &buffer);
    if (result == 0)
    {
        if (buffer.st_mode & S_IFREG)
            return (1);
        else if (buffer.st_mode & S_IFDIR)
            return (2);
        else
            return (3);
    }
    else
        return (-1);
}

int	configFile::checkFile(std::string const path, int mode)
{
    return (access(path.c_str(), mode));
}

int configFile::isFileExistAndReadable(std::string const path, std::string const index)
{
    if (getTypePath(index) == 1 && checkFile(index, 4) == 0)
        return (0);
    if (getTypePath(path + index) == 1 && checkFile(path + index, 4) == 0)
        return (0);
    return (-1);
}

std::string	configFile::readFile(std::string path)
{
    if (path.empty() || path.length() == 0)
        return (NULL);
    std::ifstream config_file(path.c_str());
    if (!config_file || !config_file.is_open())
        return (NULL);

    std::stringstream stream_binding;
    stream_binding << config_file.rdbuf();
    return (stream_binding.str());
}

/* Get functions */
std::string configFile::getPath()
{
    return (this->_path);
}

int configFile::getSize()
{
    return (this->_size);
}