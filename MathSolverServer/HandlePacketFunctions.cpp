#include "HandlePacketFunctions.h"

void DisconnectClient(uint32_t id, Packet* p)
{
	ServerManager::GetInstance()->Disconnect(id);
}

void HandlePing(uint32_t id, Packet* p)
{
	std::string roomCode, username;
	(*p) >> roomCode;
	(*p) >> username;
	ServerManager::GetInstance()->AddClient(id, username);
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	packet->GetHeader().packetType = RoomResponse;

	// create new room
	if (roomCode == "") {
		roomCode = ServerManager::GetInstance()->CreateRoom(id);
		(*packet) << id;
		(*packet) << roomCode;
		ServerManager::GetInstance()->GetServer()->SendTcp(std::move(packet), id);
		ServerManager::GetInstance()->SetParticipentType(id, ParticipentType::Host);
	}
	else {
		// try to join room
		if (!ServerManager::GetInstance()->AddToRoom(roomCode, id)) {
			ServerManager::GetInstance()->Disconnect(id, false);
			return;
		}
		else {
			(*packet) << id;
			(*packet) << roomCode;
			ServerManager::GetInstance()->GetServer()->SendTcp(std::move(packet), id);
			ServerManager::GetInstance()->SendRoomClientsToConnectedUser(id);
		}
	}
}

void HandleAudio(uint32_t id, Packet* p)
{
	ServerManager::GetInstance()->ActivateUdpOk(id);
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	// add id to the packet
	*packet = *p;
	(*packet) << id;
	//ServerManager::GetInstance()->GetServer()->SendAllUdp(std::move(packet), id);
	//ServerManager::GetInstance()->SendFromTo(id, id, packet, 1);
	ServerManager::GetInstance()->SendToRoomOf(id, std::move(packet), 1);
}

void HandleLine(uint32_t id, Packet* p)
{
	// make sure the host sent this
	if (ServerManager::GetInstance()->GetParticipentType(id) != ParticipentType::Host)
		return;
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	packet->GetHeader().packetType = SendLine;
	uint32_t to;
	(*p) >> to;
	packet->WriteArr(p->GetData() + sizeof(uint32_t), p->GetSize() - sizeof(uint32_t));
	ServerManager::GetInstance()->SendFromTo(id, to , std::move(packet));
}

void HandleCell(uint32_t id, Packet* p)
{
	// make sure editor sent this
	if (ServerManager::GetInstance()->GetParticipentType(id) < ParticipentType::Editor)
		return;
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	*packet = *p;
	//ServerManager::GetInstance()->GetServer()->SendAllTcp(std::move(packet), id);
	ServerManager::GetInstance()->SendToRoomOf(id, std::move(packet));
}

void HandleParticipentTypeChanged(uint32_t id, Packet* p)
{
	// make sure host sent this
	if (ServerManager::GetInstance()->GetParticipentType(id) != ParticipentType::Host)
		return;
	uint32_t uid;
	(*p) >> uid;
	ParticipentType temp;
	(*p) >> temp;
	ServerManager::GetInstance()->SetParticipentType(uid, temp);
}


