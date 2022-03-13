#include "PacketExecutor.h"

PacketExecutor::PacketExecutor()
{
}

PacketExecutor* PacketExecutor::GetInstance()
{
    if (instance == NULL)
        PacketExecutor::instance = new PacketExecutor();
    return instance;
}

void PacketExecutor::HandlePacket(uint32_t id,Packet* packet)
{
    if (!packet)
        return;
    mutex.lock();
    packetQueue.push({ id, packet });
    mutex.unlock();
}

void PacketExecutor::Udate()
{
    mutex.lock();
    while (!packetQueue.empty()) {
        tempQueue.push(packetQueue.front());
        packetQueue.pop();
    }
    mutex.unlock();

    while (!tempQueue.empty()) {
        Packet* tempP = tempQueue.front().second;
        
        packetFunctions[tempP->GetHeader().packetType](tempQueue.front().first,tempP);

        delete tempP;

        tempQueue.pop();
    }
}

void PacketExecutor::SetFunctions(vector<std::function<void(uint32_t, Packet*)>> packetFunctions)
{
    this->packetFunctions.assign(packetFunctions.begin(), packetFunctions.end());
}

