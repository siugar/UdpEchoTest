#include "UdpServer.h"

#include <arpa/inet.h>
#include <netinet/in.h>

int32_t CUdpServer::init(std::string_view ip, int32_t port) {

	// socket address used for the server
	struct sockaddr_in serverAddress {};

	serverAddress.sin_family = AF_INET;

	// htons: host to network short: transforms a value in host byte
	// ordering format to a short value in network byte ordering format
	serverAddress.sin_port = htons(port);

	::inet_pton(AF_INET, ip.data(), &serverAddress.sin_addr.s_addr);

	// create a UDP socket, creation returns -1 on failure	
	if ((m_Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("could not create socket\n");
		return -1;
	}

	// bind it to listen to the incoming connections on the created server
	// address, will return -1 on error
	if ((bind(m_Socket, (struct sockaddr*)&serverAddress,
			  sizeof(serverAddress))) < 0) {
		printf("could not bind socket\n");
		return -2;
	}

	printf("Listening %s\n", inet_ntoa(serverAddress.sin_addr));

	printf("Echo server is listening %s:%d\n", ip.data(), port);

	return 0;
}

void CUdpServer::update() {
	// socket address used to store client address
	struct sockaddr_in clientAddress {};
	//clientAddress.sin_family = AF_INET;

	socklen_t clientAddressLen = sizeof(clientAddress);

	char buffer[500] = {0};
	// read content into buffer from an incoming client
	int len = recvfrom(m_Socket, buffer, sizeof(buffer), 0,
					   (struct sockaddr*)&clientAddress,
					   &clientAddressLen);

	// inet_ntoa prints user friendly representation of the
	// ip address
	buffer[len] = '\0';
	printf("received: '%s' from client %s\n", buffer,
		   inet_ntoa(clientAddress.sin_addr));

	// send same content back to the client ("echo")
	sendto(m_Socket, buffer, len, 0, (struct sockaddr*)&clientAddress,
		   sizeof(clientAddress));

}

