#include "HandlePacketFunctions.h"

void DisconnectClient(uint32_t id, Packet* p)
{
	ServerManager::GetInstance()->Disconnect(id);
}

void HandlePing(uint32_t id, Packet* p)
{
	std::string roomCode;
	(*p) >> roomCode;
	ServerManager::GetInstance()->AddClient(id);
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	packet->GetHeader().packetType = RoomResponse;
	if (roomCode == "") {
		roomCode = ServerManager::GetInstance()->CreateRoom(id);
	}
	else {
		if (!ServerManager::GetInstance()->AddToRoom(roomCode, id)) {
			ServerManager::GetInstance()->Disconnect(id, false);
			return;
		}

	}
	(*packet) << roomCode;
	ServerManager::GetInstance()->GetServer()->SendTcp(std::move(packet), id);
}

void HandleAudio(uint32_t id, Packet* p)
{
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	*packet = *p;
	//ServerManager::GetInstance()->GetServer()->SendAllUdp(std::move(packet), id);
	ServerManager::GetInstance()->SendToRoomOf(id, std::move(packet), 1);
}

void HandleLine(uint32_t id, Packet* p)
{
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	packet->GetHeader().packetType = SendLine;
	uint32_t to;
	(*p) >> to;
	packet->WriteArr(p->GetData() + sizeof(uint32_t), p->GetSize() - sizeof(uint32_t));
	ServerManager::GetInstance()->SendFromTo(id, to , std::move(packet));
}

void HandleCell(uint32_t id, Packet* p)
{
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	*packet = *p;
	//ServerManager::GetInstance()->GetServer()->SendAllTcp(std::move(packet), id);
	ServerManager::GetInstance()->SendToRoomOf(id, std::move(packet));
}


