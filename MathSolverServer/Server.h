#pragma once
#include "TCP.h"
#include "UDP.h"

class Server
{
public:
	Server(size_t maxClients);
	void StartServer(int16_t port);
	void ConnectClient(std::shared_ptr<TCP> sptr, asio::error_code& errorCode);
	void SendUdp(Packet& packet, uint32_t id);
	void SendAllUdp(Packet& packet, uint32_t exp = UINT32_MAX);
	void SendTcp(Packet& packet, uint32_t id);
	void SendAllTcp(Packet& packet, uint32_t exp = UINT32_MAX);
	void Disconnect(uint32_t id);


private:
	asio::io_service service;
	asio::ip::udp::socket* udpSocket;
	std::thread serviceThread;
	asio::ip::tcp::acceptor acceptor;
	vector<std::pair<std::shared_ptr<TCP>, std::shared_ptr<UDP>>> connections;
};

