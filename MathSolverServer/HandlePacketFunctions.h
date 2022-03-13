#pragma once
#include "Packet.h"
#include "ServerManager.h"

void DisconnectClient(uint32_t id ,Packet* p);
void HandlePing(uint32_t id, Packet* p);

#define CreatePacketVector {DisconnectClient,HandlePing}