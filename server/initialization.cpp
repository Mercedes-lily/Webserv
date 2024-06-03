#include "../inc/initialization.hpp"

using std::string;
using std::cout;
using std::endl;

int closefile(const string &str, int file)
{
	cout << str << endl;
	close(file);
	return (1);
}

int closefile(const string &str, int file, int file2)
{
	cout << str << endl;
	close(file);
	close (file2);
	return (1);
}

int	nblock_connection(int fd, int reuse, int type)
{	
	int flag;
	flag = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) == -1)
	{
		closefile("F_SETFL failed", fd);
		return(INTERNAL_SERVER_ERROR);//TODO check error code;
	}
	if (type == 1)
	{
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) > 0)
		{
			closefile("Error with setsockopt", fd);
			return(INTERNAL_SERVER_ERROR);//TODO check error code;
		}
	}
	return(0);
}

int serverInitialization(serverConfig &serv)
{
	serv.setAddress();
	serv.setFd(socket(AF_INET, SOCK_STREAM, 0));
	if (serv.getFd() == -1)
	{
		closefile("socket failed", serv.getFd());
		return(INTERNAL_SERVER_ERROR);
	}
	return(0);
}

int bindNlisten(serverConfig &serv)
{
	const struct sockaddr *add = serv.getSockaddr();
	if(bind(serv.getFd(), add, sizeof(const struct sockaddr)) < 0)
	{
		closefile("bind failed", serv.getFd());
		return(INTERNAL_SERVER_ERROR);
	}
	if (listen(serv.getFd(), serv.getBacklog()) < 0)  //TODO add poll and change the place we initialize pollfd
	{
		closefile("listen failed", serv.getFd());
		return(INTERNAL_SERVER_ERROR);
	}
	return(0);
}

int pollfdInitialization(serverConfig &serv, struct pollfd *pollfds)
{
	pollfds->fd = serv.getFd();
	pollfds->events = POLLIN;
	pollfds->revents = 0;
	return(0);
}

int initialization(serverConfig &serv, int reuse, struct pollfd *pollfds)
{
	int code = 0;

	code = serverInitialization(serv);
	if(code != 0)
		return(code);
	//Non-blocking connection using fcntl
	code = nblock_connection(serv.getFd(), reuse, 1);
	if(code != 0)
		return(code);
	//Bind and listen
	code = pollfdInitialization(serv, pollfds);
	if(code != 0)
		return(code);
	//Pollfd initialization
	code = bindNlisten(serv);
	return(code);
}