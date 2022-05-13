#pragma once
#include <share.h>
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <queue>
#include <unordered_map>
#include <mutex>
#include "Packet.h"
#include "PacketExecutor.h"

class NetworkProtocol
{
public:
	NetworkProtocol(asio::io_service& service);
	virtual void Send(std::shared_ptr<Packet> packet);
	virtual void SetId(uint32_t id);
	uint32_t GetId();
protected:
	virtual void Write() = 0;
	asio::io_service& service;
	asio::io_service::strand SendStrand;
	std::queue<std::shared_ptr<Packet>> sendQueue;
	uint32_t id;
};