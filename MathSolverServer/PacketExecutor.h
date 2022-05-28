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
    /// <summary>
    /// get the singelton instance
    /// </summary>
    /// <returns></returns>
    static PacketExecutor* GetInstance();

    /// <summary>
    /// handle a packet by adding it to the queue
    /// </summary>
    /// <param name="id"></param>
    /// <param name="packet"></param>
    void HandlePacket(uint32_t id, Packet* packet);

    /// <summary>
    /// do all the packets in the queue
    /// </summary>
    void Udate();

    /// <summary>
    /// set the packet functions 
    /// </summary>
    /// <param name="packetFunctions">the functions</param>
    void SetFunctions(vector<std::function<void(uint32_t, Packet*)>> packetFunctions);

private:
    /// <summary>
    /// constructor
    /// </summary>
    PacketExecutor();

    /// <summary>
    /// singelton instance
    /// </summary>
    static PacketExecutor* instance;

    PacketExecutor(PacketExecutor& other) = delete;
    void operator=(const PacketExecutor&) = delete;

    /// <summary>
    /// mutex for thread safety
    /// </summary>
    sf::Mutex mutex;

    /// <summary>
    /// the queue
    /// </summary>
    std::queue< std::pair<uint32_t, Packet*>> packetQueue;

    /// <summary>
    /// temp queue
    /// </summary>
    std::queue< std::pair<uint32_t, Packet*>> tempQueue;

    /// <summary>
    /// the functions
    /// </summary>
    vector<std::function<void(uint32_t, Packet*)>> packetFunctions;
};

