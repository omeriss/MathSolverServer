#pragma once
#include <queue>
#include <functional>

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <SFML//System/Mutex.hpp>
#include "Packet.h"

class PacketExecutor
{
public:

    static PacketExecutor* GetInstance();

    void HandlePacket(uint32_t id, Packet* packet);

    void Udate();

    void SetFunctions(vector<std::function<void(uint32_t, Packet*)>> packetFunctions);

private:
    PacketExecutor();
    static PacketExecutor* instance;
    PacketExecutor(PacketExecutor& other) = delete;
    void operator=(const PacketExecutor&) = delete;
	sf::Mutex mutex;
	std::queue< std::pair<uint32_t, Packet*>> packetQueue;
    std::queue< std::pair<uint32_t,Packet*>> tempQueue;
    vector<std::function<void(uint32_t ,Packet*)>> packetFunctions;
};

