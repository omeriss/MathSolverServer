#pragma once
#include "NetworkProtocol.h"

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
		this->map[s] = val;
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

class UDP :
	public NetworkProtocol
{
public:
	static asio::ip::udp::socket* socket;
	static UdpReadData readData;
	static void ReadPacket();
	UDP(asio::io_service& s, asio::ip::udp::endpoint endpoint, uint32_t id = 0);
	~UDP();
	asio::ip::udp::endpoint& GetEndPint();

private:
	void Write();
	asio::ip::udp::endpoint endpoint;
};

