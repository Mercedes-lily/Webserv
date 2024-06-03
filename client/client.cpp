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
	//send(client_fd, "GET /info.html HTTP/1.1\r\n\r\n", 25, 0);
	//User-Agent: custom-client		Information about the client
	//Host: weebserv		If wrong return an error or change it for the real host name
	//Accept-Language: fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5 		Specifies the preferred languages for the response content. Servers can use this header for content negotiation. The server can determine the best language to use for the response content based on the client's preferences.
	//Connection: Keep-Alive		Connection: Keep-Alive: Indicates that the client wishes to keep the connection open for further requests. This header enables HTTP persistent connections, allowing multiple requests and responses to be sent over a single TCP connection. It can reduce latency and improve performance by avoiding the overhead of establishing a new connection for each request.
	//avec fichier
	//send(client_fd, "GET /bin/default.html?color=purple&Host=localhost HTTP/1.1\nUser-Agent: custom-client\nAccept-Language: fr-CA, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5\nConnection: keep-alive\r\n\r\n", 162, 0);
	
	//dans un dossier sans default
	//send(client_fd, "GET /script?color=purple&Host=localhost HTTP/1.1\nUser-Agent: custom-client\nAccept-Language: fr-CA, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5\nConnection: keep-alive\r\n\r\n", 162, 0);
	
	//query 
	// send(client_fd, "DELETE /test2.txt HTTP/1.1\nHost: localhost\nContent-Type: asas\r\n\r\n", 162, 0);
	
	//default dans un dossier
	send(client_fd, "GET / HTTP/1.1\nUser-Agent: custom-client\nHost : localhost\nAccept-Language: fr-CA, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5\nConnection: keep-alive\r\n\r\n", 162, 0);
	
	//default
	//send(client_fd, "GET / HTTP/1.1\nUser-Agent: custom-client\nHost: weebserv\nAccept-Language: fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5\nConnection: Keep-Alive\r\n\r\n", 144, 0);
	
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