#pragma once
#include "Packet.h"
#include "ServerManager.h"

void DisconnectClient(uint32_t id ,Packet* p);
void HandlePing(uint32_t id, Packet* p);
void HandleAudio(uint32_t id, Packet* p);
void HandleLine(uint32_t id, Packet* p);
void HandleCell(uint32_t id, Packet* p);
void HandleParticipentTypeChanged(uint32_t id, Packet* p);

#define CreatePacketVector {DisconnectClient,HandlePing, HandleAudio, HandleLine, HandleCell, HandleParticipentTypeChanged}