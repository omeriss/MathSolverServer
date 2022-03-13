#include "HandlePacketFunctions.h"

void DisconnectClient(uint32_t id, Packet* p)
{
	ServerManager::GetInstance()->Disconnect(id);
}

void HandlePing(uint32_t id, Packet* p)
{
	std::cout << id;
	ServerManager::GetInstance()->AddClient(id);
	Packet packet;
	packet.GetHeader().packetType = PingBack;
	ServerManager::GetInstance()->GetServer()->SendTcp(packet, id);
}

