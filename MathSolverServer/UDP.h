#pragma once
#include <share.h>
#include "Packet.h"
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <queue>
#include "PacketExecutor.h"

#define MaxPacketSize 4096


class UDP :std::enable_shared_from_this<UDP>
{
public:
	UDP(asio::io_service& s, asio::ip::udp::socket* sock, asio::ip::udp::endpoint endpoint, uint32_t id = 0);
	~UDP();
	asio::ip::udp::socket& GetSocket();
	void ReadPacket();
	void Send(Packet& packet);
	void SetId(uint32_t id);
	void Disconnect();
	uint32_t GetId();

private:
	void Write();
	asio::ip::udp::endpoint endpoint;
	asio::io_service& service;
	asio::io_service::strand SendStrand;
	asio::ip::udp::socket* socket;
	std::queue<Packet> sendQueue;
	std::array<BYTE, MaxPacketSize> buffer;
	uint32_t id;
};

