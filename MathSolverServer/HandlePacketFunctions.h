#pragma once
#include "Packet.h"
#include "ServerManager.h"

/// <summary>
/// disconnect client
/// </summary>
/// <param name="id"> the id of the client </param>
/// <param name="p"> the packet </param>
void DisconnectClient(uint32_t id ,Packet* p);

/// <summary>
/// handle room request from a client
/// </summary>
/// <param name="id"> the id of the client </param>
/// <param name="p"> the packet </param>
void HandlePing(uint32_t id, Packet* p);

/// <summary>
/// handle autio from the client
/// </summary>
/// <param name="id"> the id of the client </param>
/// <param name="p"> the packet </param>
void HandleAudio(uint32_t id, Packet* p);

/// <summary>
/// handle line from a host 
/// </summary>
/// <param name="id"> the id of the client </param>
/// <param name="p"> the packet </param>
void HandleLine(uint32_t id, Packet* p);

/// <summary>
/// handle cell change from a editor client
/// </summary>
/// <param name="id"> the id of the client </param>
/// <param name="p"> the packet </param>
void HandleCell(uint32_t id, Packet* p);

/// <summary>
/// handle host changing participent type
/// </summary>
/// <param name="id"> the id of the client </param>
/// <param name="p"> the packet </param>
void HandleParticipentTypeChanged(uint32_t id, Packet* p);

#define CreatePacketVector {DisconnectClient,HandlePing, HandleAudio, HandleLine, HandleCell, HandleParticipentTypeChanged}