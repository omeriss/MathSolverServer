#pragma once
#include <share.h>
#include "Packet.h"
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <queue>
#include <unordered_map>
#include <mutex>
#include "PacketExecutor.h"

#define MaxPacketSize 4096

template <typename KeyType, typename ValType>
class TSUM {
public:
	ValType Get(KeyType& s) {
		lock.lock();
		auto temp = map.find(s);
		ValType ans = UINT32_MAX;
		if (temp != map.end())
			ans = (*temp).second;
		lock.unlock();
		return ans;
	}
	void Remove(KeyType& s) {
		lock.lock();
		map.erase(s);
		lock.unlock();
	}
	void Set(KeyType& s, ValType val) {
		lock.lock();
		this-> map[s] = val;
		lock.unlock();
	}
private:
	std::unordered_map<KeyType, ValType> map;
	std::mutex lock;
};

struct UdpReadData {

	TSUM<std::string, uint32_t> clientsMap;
	std::array<BYTE, MaxPacketSize> buffer;
	asio::ip::udp::endpoint reciveEndPoint;
};

class UDP :std::enable_shared_from_this<UDP>
{
public:
	static asio::ip::udp::socket* socket;
	static UdpReadData readData;
	static void ReadPacket();
	UDP(asio::io_service& s, asio::ip::udp::endpoint endpoint, uint32_t id = 0);
	~UDP();
	void Send(std::shared_ptr<Packet> packet);
	void SetId(uint32_t id);
	asio::ip::udp::endpoint& GetEndPint();
	uint32_t GetId();

private:
	void Write();
	asio::ip::udp::endpoint endpoint;
	asio::io_service& service;
	asio::io_service::strand SendStrand;
	std::queue<std::shared_ptr<Packet>> sendQueue;
	uint32_t id;
};

