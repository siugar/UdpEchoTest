// EchoClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <time.h>
//#include <format>

#include "UdpClient.h"

struct CArgus
{
	std::string ip = "127.0.0.1";
	int32_t port = 1212;
	std::string message = "This is the message from client!!!";
	int32_t maxRetry = 10;
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
		} else if (argu == "--message") {
			if ((i + 1) < argc) {
				argus.message = argv[i + 1];
				i += 2;
			}
		} else if (argu == "--max_retry") {
			if ((i + 1) < argc) {
				argus.maxRetry = atoi(argv[i + 1]);
				i += 2;
			}
		} else {
			++i;
		}
	}
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
std::string getCurrentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);

	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return buf;
}

int main(int argc, char* argv[]) {
	CArgus argus;
	handleArguemnt(argc, argv, argus);
	char echoMsg[512] = {0};
	//std::string echoMsg = std::format("{} {}", argus.message, getCurrentDateTime());		
	sprintf(echoMsg, "%s %s", argus.message.data(), getCurrentDateTime().data());


	CUdpClient client;

	if (client.init(argus.ip, argus.port, argus.maxRetry) < 0) {
		printf("Fail to init the server\n");
		return -1;
	}

	auto result = client.sendMsg(echoMsg);
	printf("send msg result %d\n", result);

	while (1) {
		client.update();
		const auto echoState = client.getState();
		if (echoState == CUdpClient::EchoState::eRECEIVED) {
			printf("Echo client leave normally\n");
			return 0;
		} else if (echoState == CUdpClient::EchoState::eREACH_MAX_TRY) {
			printf("Echo client leave abnormally because of reaching the maximal retry count\n");
			return 1;
		}
	}

	return -2;
}

