#pragma once

#include <string>
#include <cstdint>

class CUdpServer
{
public:

  int32_t init(std::string_view ip, int32_t port);
  void update();





private:
	int m_Socket{};



};
