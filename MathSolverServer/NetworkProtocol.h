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
	/// <summary>
	/// constructor
	/// </summary>
	/// <param name="service"> the io service </param>
	NetworkProtocol(asio::io_service& service);

	/// <summary>
	/// send packet
	/// </summary>
	/// <param name="packet"></param>
	virtual void Send(std::shared_ptr<Packet> packet);

	/// <summary>
	/// set id
	/// </summary>
	/// <param name="id"> the new id</param>
	virtual void SetId(uint32_t id);

	/// <summary>
	/// get id
	/// </summary>
	/// <returns> the id</returns>
	uint32_t GetId();
protected:
	/// <summary>
	/// write the packet
	/// </summary>
	virtual void Write() = 0;

	/// <summary>
	/// ref to the service
	/// </summary>
	asio::io_service& service;

	/// <summary>
	/// makes sure no two packet are sent to the same place at the same time
	/// </summary>
	asio::io_service::strand SendStrand;

	/// <summary>
	/// out packet queue
	/// </summary>
	std::queue<std::shared_ptr<Packet>> sendQueue;

	/// <summary>
	/// id
	/// </summary>
	uint32_t id;
};