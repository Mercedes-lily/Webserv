#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <poll.h>

#define PORT 8080


#include <vector>

std::vector<unsigned char> readImageFile(const std::string& filename) {
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    // Determine the file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a buffer to hold the file data
    std::vector<unsigned char> buffer(size);

    // Read the file data into the buffer
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Error reading file");
    }

    // Close the file
    file.close();

    return buffer;
}

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
	// std::vector<unsigned char> imageData;
    // try {
    //     // Path to the image file
    //     std::string imagePath = "/Users/seroy/Desktop/webserv2.0/client/image.png";

    //     // Get the binary data from the image file
    //     imageData = readImageFile(imagePath);

    //     // Print the size of the binary data
    //     std::cout << "Image size: " << imageData.size() << " bytes" << std::endl;

    //     // Optionally, you can process the binary data here
    // } catch (const std::exception& e) {
    //     std::cerr << "Error: " << e.what() << std::endl;
    //     return 1;
    // }
	// std::string body;
	// for (std::vector<unsigned char>::iterator it = imageData.begin();it != imageData.end(); it++)
	// 	body += *it;
	// std::string response;
	// response += "POST /image.png HTTP/1.1\nUser-Agent: custom-client\nHost : localhost\nContent-Length: ";
	// response += std::to_string(body.size());
	// std::cout << "body length: "<< body.size() << std::endl;
	// std::cout << "body: "<< body << std::endl;
	// response += "\nConnection: keep-alive\r\n";
	// response += body;
	// response += "/r/n\r\n";
	// send(client_fd, response.c_str(), response.size(), 0);
	send(client_fd, "POST /temp.txt HTTP/1.1\nUser-Agent: custom-client\nHost : localhost\nContent-Length: 22\nConnection: keep-alive\r\nJe m'appelle Thiboutot/r/n\r\n", 162, 0);
	char buffer[20000] = {0};
	printf("hello message sent\n");
	//
	int	valread = read(client_fd, buffer, 20000);
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
