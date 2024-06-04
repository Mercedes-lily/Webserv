#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <poll.h>

#define PORT 8080

int	main()
{
	int	client_fd	= socket(AF_INET, SOCK_STREAM, 0); //try catch
	const int port = PORT;
	struct sockaddr_in	address;
	int	addrlen = sizeof(address);

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (connect(client_fd, (struct sockaddr*)&address, addrlen) < 0)
	{
		perror("connect failed\n"); //TODO À changer
		close(client_fd);
		return (1);
	}
	send(client_fd, "POST /temp.txt HTTP/1.1\nUser-Agent: custom-client\nHost : localhost\nContent-Length: 22\nConnection: keep-alive\r\nJe m'appelle Thiboutot/r/n\r\n", 162, 0);
	char buffer[10000] = {0};
	printf("hello message sent\n");
	//
	int	valread = read(client_fd, buffer, 10000);
	if (valread < 0)
	{
		perror("read failed\n"); //TODO À changer
		close(client_fd);
		return (1);
	}
	printf("message received: %s\n", buffer);
	close(client_fd);
	return (0);
}