#pragma once
#include "NetworkProtocol.h"

class TCP
	:public NetworkProtocol
{
public:
	TCP(asio::io_service& service);
	asio::ip::tcp::socket& GetSocket();
	~TCP();
	void ReadPacket();
	void Disconnect();
private:
	void Write();
	asio::ip::tcp::socket socket;
};

