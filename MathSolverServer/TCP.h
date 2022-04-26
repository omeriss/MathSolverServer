#pragma once
#include <share.h>
#include "Packet.h"
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <queue>
#include "PacketExecutor.h"

class TCP
{
public:
	TCP(asio::io_service& service);
	asio::ip::tcp::socket& GetSocket();
	~TCP();
	void ReadPacket();
	void Send(std::shared_ptr<Packet> packet);
	void SetId(uint32_t id);
	uint32_t GetId();
	void Disconnect();
private:
	void Write();
	asio::io_service& service;
	asio::io_service::strand SendStrand;
	asio::ip::tcp::socket socket;
	std::queue<std::shared_ptr<Packet>> sendQueue;
	uint32_t id;
};

