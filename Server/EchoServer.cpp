// EchoServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "UdpServer.h"

struct CArgus
{
	std::string ip = "127.0.0.1";
	int32_t port = 1212;	
};

void handleArguemnt(int argc, char* argv[], CArgus& argus) {

	int32_t i = 1;

	while (i < argc) {
		std::string argu = argv[i];
		if (argu == "--ip") {
			if ((i + 1) < argc) {
				argus.ip = argv[i + 1];
				i += 2;
			}
		} else if (argu == "--port") {
			if ((i + 1) < argc) {
				argus.port = atoi(argv[i + 1]);
				i += 2;
			}
		} else {
			++i;
		}
	}
}


int main(int argc, char* argv[]) {
	CArgus argus;

	handleArguemnt(argc, argv, argus);

	CUdpServer server;

	if (server.init(argus.ip, argus.port) < 0) {
		printf("Fail to init the server\n");
		return -1;
	}

	while (1) {
		server.update();
	}

	return 0;
}


