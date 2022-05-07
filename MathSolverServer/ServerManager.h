#pragma once
#include "Server.h"
#include "Constants.h"
#include <map>

enum class ParticipentType:uint16_t {
	WatchOnly = 0, Editor = 1, Host = 2
};

struct ClientInfo {
	uint32_t id;
	std::string roomCode;
	ParticipentType participentType = ParticipentType::WatchOnly;
	bool UdpOk = false;
};

struct RoomInfo {
	vector<ClientInfo*> clients;
	ClientInfo* host;
};


class ServerManager
{
public:
	/// <summary>
	/// This function is used to get a pointer to the singleton instance of this class
	/// </summary>
	/// <returns> The pointer to the instance of this class</returns>
	static ServerManager* GetInstance();
	/// <summary>
	/// Start the server
	/// </summary>
	/// <param name="port"> The port the server will start on</param>
	/// <param name="maxClients"> The max amount of clients that can be connected at once</param>
	void Start(int16_t port = 16016, size_t maxClients = 30);
	/// <summary>
	/// Get the instace to the server
	/// </summary>
	/// <returns> The server object</returns>
	Server* GetServer();
	/// <summary>
	/// Disconnect a clinet from the server
	/// </summary>
	/// <param name="id"> the id of the client we want to disconnect</param>
	/// <param name="remove"> should the client object be removed</param>
	void Disconnect(uint32_t id, bool remove = true);
	/// <summary>
	/// Add a new clinet
	/// </summary>
	/// <param name="id"> The id of the new client</param>
	void AddClient(uint32_t id);
	/// <summary>
	/// create a new room
	/// </summary>
	/// <param name="host"> the id of the host of the room</param>
	/// <returns> the room code (string)</returns>
	std::string CreateRoom(uint32_t host);
	/// <summary>
	/// add a client to a room
	/// </summary>
	/// <param name="roomCode"> room code</param>
	/// <param name="id"> the id of the client</param>
	/// <returns> was the client added to the room (bool)</returns>
	bool AddToRoom(std::string roomCode, uint32_t id);
	/// <summary>
	/// send a packet to the room of a client
	/// </summary>
	/// <param name="id"> the id of the client we want to send the packet to his room</param>
	/// <param name="packet"> the packet we want to send</param>
	/// <param name="protocol"> the protocol we want to sent the packet with (tcp-0, udp-1)</param>
	/// <param name="include"> if the func will send it to the sender too</param>
	void SendToRoomOf(uint32_t id, std::shared_ptr<Packet> packet, int protocol = 0, bool include = false);
	/// <summary>
	/// send a packet from a client to another client only if they are in the same room
	/// </summary>
	/// <param name="from"> the id of the client that sends the massage</param>
	/// <param name="to"> the id of the clinet we want to send the packet to</param>
	/// <param name="packet"> the packet we want to send</param>
	/// <param name="protocol"> the protocol we want to sent the packet with (tcp-0, udp-1)</param>
	void SendFromTo(uint32_t from, uint32_t to, std::shared_ptr<Packet> packet, int protocol = 0);
	/// <summary>
	/// set the udp ok of a client to true
	/// </summary>
	/// <param name="id"> the id of the client</param>
	void ActivateUdpOk(uint32_t id);
	/// <summary>
	/// Get the participent type of a client
	/// </summary>
	/// <param name="id"> the id of the client</param>
	/// <returns> the participent type of the client (enum)</returns>
	ParticipentType GetParticipentType(uint32_t id);
	/// <summary>
	/// Get the participent type of a client
	/// </summary>
	/// <param name="id"> the id of the client</param>
	/// <returns> the participent type of the client (enum)</returns>
	void SetParticipentType(uint32_t id, ParticipentType participentType);
private:
	std::map<std::string, RoomInfo> rooms;
	static ServerManager* instance;
	vector<ClientInfo*> clientsData;
	Server* server;
};

