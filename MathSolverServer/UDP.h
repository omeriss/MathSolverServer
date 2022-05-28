#pragma once
#include "NetworkProtocol.h"

#define MaxPacketSize 4096

template <typename KeyType, typename ValType>
class TSUM {
public:

	/// <summary>
	/// get value
	/// </summary>
	/// <param name="s"> the key</param>
	/// <returns></returns>
	ValType Get(KeyType& s) {
		lock.lock();
		auto temp = map.find(s);
		ValType ans = UINT32_MAX;
		if (temp != map.end())
			ans = (*temp).second;
		lock.unlock();
		return ans;
	}

	/// <summary>
	/// remove value
	/// </summary>
	/// <param name="s">the key</param>
	void Remove(KeyType& s) {
		lock.lock();
		map.erase(s);
		lock.unlock();
	}

	/// <summary>
	/// set value
	/// </summary>
	/// <param name="s">key</param>
	/// <param name="val">value</param>
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
	/// <summary>
	/// static socket
	/// </summary>
	static asio::ip::udp::socket* socket;

	/// <summary>
	/// the read data struct
	/// </summary>
	static UdpReadData readData;

	/// <summary>
	/// read packet - one full all client cuz there is no connetion
	/// </summary>
	static void ReadPacket();

	/// <summary>
	/// constructor
	/// </summary>
	/// <param name="s">the service for async</param>
	/// <param name="endpoint">the endpoint of client or server</param>
	/// <param name="id">the id of client or server</param>
	UDP(asio::io_service& s, asio::ip::udp::endpoint endpoint, uint32_t id = 0);
	~UDP();

	/// <summary>
	/// get the endpoint
	/// </summary>
	/// <returns>the endpoint</returns>
	asio::ip::udp::endpoint& GetEndPint();

private:
	/// <summary>
	/// write the packet
	/// </summary>
	void Write();

	/// <summary>
	/// the endpoint
	/// </summary>
	asio::ip::udp::endpoint endpoint;
};

