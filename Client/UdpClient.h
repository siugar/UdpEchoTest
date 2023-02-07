#pragma once

#include <string>
#include <string_view>
#include <cstdint>

class CUdpClient
{
public:
	enum class EchoState
	{
		eNONE,
		eFAIL,
		eSENT,

		eRECEIVED,
		eREACH_MAX_TRY
	};

	int32_t init(std::string_view ip, int32_t port, int32_t maxRetryCount);
	int32_t sendMsg(std::string_view message);

	void update();

	EchoState getState() const;
	bool isFailed() const;

private:
	void resendMsg();
	int32_t getWaitTimeMs(int32_t failCount) const;



private:
	int32_t m_MaxRetryCount{};
	int32_t m_RetryCount{};

	std::string m_SendingMsg;

	int m_Socket{};
	EchoState m_State{};




};
