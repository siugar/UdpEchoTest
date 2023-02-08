#include "UdpClient.h"

#include <chrono>
#include <thread>
#include <math.h>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

constexpr int32_t BUFFER_SIZE = 512;

int32_t getRand(int32_t sideCount)
{
	if(sideCount<=0) {
		printf("sideCount %d should >0", sideCount);
		return 0;
	}
	
    return (1 + std::rand()/((RAND_MAX + 1u)/sideCount));
}

int32_t CUdpClient::init(std::string_view ip, int32_t port, int32_t maxRetryCount) {
	m_MaxRetryCount = maxRetryCount;
	
	m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in serverAddr{};
		
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	
	::inet_pton(AF_INET, ip.data(), &serverAddr.sin_addr.s_addr);
	
	if (::connect(m_Socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
		printf("connect error!\n");
		return -1;
	}
	
	printf("target ip %s port:%d, m_Socket = %d\n", ip.data(), port, m_Socket);
	
	std::srand(std::time(nullptr));

	return 0;
}

int32_t CUdpClient::sendMsg(std::string_view message) {

    struct iovec iov[1];
    iov[0].iov_base = (void*)(message.data());
    iov[0].iov_len = message.length();

    struct msghdr messagePacket;
	messagePacket.msg_name = nullptr;
    messagePacket.msg_namelen = 0;
    messagePacket.msg_iov = iov;
    messagePacket.msg_iovlen = 1;
    messagePacket.msg_control = 0;
    messagePacket.msg_controllen = 0;

	auto msgSendSize = ::sendmsg(m_Socket, &messagePacket, 0);
	printf("sendMsg size = %d\n", msgSendSize);
	
	m_sendingMsg = message;
	if(msgSendSize == message.length()) {
		m_State = EchoState::eSENT;	
		return 0;
	} else {
		m_State = EchoState::eFAIL;					
		//printf("m_sendingMsg = %s", m_sendingMsg.data());
		return -1;
	}    
}

void CUdpClient::resendMsg() {
	struct iovec iov[1];
    iov[0].iov_base = (void*)(m_sendingMsg.data());
    iov[0].iov_len = m_sendingMsg.length();

    struct msghdr messagePacket;
	messagePacket.msg_name = nullptr;
    messagePacket.msg_namelen = 0;
    
    messagePacket.msg_iov = iov;
    messagePacket.msg_iovlen = 1;
    messagePacket.msg_control = 0;
    messagePacket.msg_controllen = 0;
    
	auto msgSendSize = ::sendmsg(m_Socket, &messagePacket, 0);
	//printf("resendMsg size = %d\n", msgSendSize);
	
	if(msgSendSize == m_sendingMsg.length()) {
		m_State = EchoState::eSENT;			
	} else {
		m_State = EchoState::eFAIL;			
	}    	
}

bool CUdpClient::isFailed() const {
	return (EchoState::eFAIL == m_State || EchoState::eREACH_MAX_TRY == m_State);
}

void CUdpClient::update() {	
    if(isFailed()) {
		if(EchoState::eFAIL == m_State && m_retryCount >=m_MaxRetryCount) {
			m_State = EchoState::eREACH_MAX_TRY;
			return;
		}
				
		const auto sleepTimeMs = getWaitTimeMs(m_retryCount);
		++m_retryCount;
		printf("retry(%d) after %d ms\n", m_retryCount, sleepTimeMs);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));		
		resendMsg();
		return;
	}


	// socket address used to store client address
	struct sockaddr_in targetAddress;
	socklen_t targetAddressLen=sizeof(targetAddress);
	
	char buffer[BUFFER_SIZE];
	// read content into buffer from an incoming client
	int len = recvfrom(m_Socket, buffer, sizeof(buffer), 0,
					   (struct sockaddr *)&targetAddress,
					   &targetAddressLen);
	
	//printf("len = %d\n", len);	
    
	if(-1 == len) {
		m_State = EchoState::eFAIL;		
		return;
	} 
	
	if(len > (sizeof(buffer)-1)) { //datagram too large for buffer: truncated
		len = sizeof(buffer)-1;
	}
	buffer[len] = '\0';	
	printf("Received(%d): '%s' from Server %s\n", len, buffer, inet_ntoa(targetAddress.sin_addr));
	m_State = EchoState::eRECEIVED;
	
}

CUdpClient::EchoState CUdpClient::getState() const {
    return m_State;
}

int32_t CUdpClient::getWaitTimeMs(int32_t failCount) const {
	constexpr int32_t RANDOM_INTERVAL_RATIO = 200; //This is my assumption = (200/1000)
	constexpr int32_t MAX_WAIT_INTERVAL_MS = 8000;//This is the requirement of the Test
	constexpr int32_t MULTIPLIER = 2;
	constexpr int32_t BASE = 500;
	/*      
		The interval between each retry basically follow the expontential backoff algorithm.
		I slightly change the random factor to prevent the the collisions of clients when the retry interval is bound to 
		the maximum 8s(The requirement in the test). The random factor of the new way is all suitable for the retry 
		interval from small to big with the maximun boundary especially.

		wait_interval = ((base * multiplier^n) <= max interval) * (1 - random_interval_ratio[0.0 ~ 0.2))	  

		Original Reference: wait_interval = (base * 2^n) +/- (random_interval)  https://www.baeldung.com/resilience4j-backoff-jitter
	*/
	
	int32_t result = BASE * pow(MULTIPLIER, failCount);
	
	//printf("result = %d\n", result);	
	result = std::clamp(result, 0, MAX_WAIT_INTERVAL_MS);
	//printf("result = %d after clamp\n", result);
	
	return result * ((1000-getRand(RANDOM_INTERVAL_RATIO)-1)/1000.f);	
}
