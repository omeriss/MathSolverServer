#pragma once
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <Python.h>
#include "Server.h"
#include "Constants.h"

enum class ParticipentType:uint16_t {
	WatchOnly = 0, Editor = 1, Host = 2
};

/// <summary>
/// struct to store clinet info
/// </summary>
struct ClientInfo {
	uint32_t id;
	std::string roomCode;
	std::string userName;
	ParticipentType participentType = ParticipentType::WatchOnly;
	bool UdpOk = false;
};

/// <summary>
/// struct to store room info
/// </summary>
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
	/// delete the instance of the severmanager
	/// </summary>
	static void DelInstance();
	/// <summary>
	/// Start the server
	/// </summary>
	/// <param name="port"> The port the server will start on</param>
	/// <param name="maxClients"> The max amount of clients that can be connected at once</param>
	/// <param name="email"> The username of the server account</param>
	/// <param name="password"> The password of the server</param>
	void Start(int16_t port = 16016, size_t maxClients = 30, std::string email = "", std::string password = "");
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
	void AddClient(uint32_t id, std::string& userName);
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
	/// send a client connected packet for all the users in the room to a client
	/// </summary>
	/// <param name="id"> the id of the client</param>
	void SendRoomClientsToConnectedUser(uint32_t id);
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
	/// <summary>
	/// create a instance
	/// </summary>
	ServerManager();
	/// <summary>
	/// delete a instance
	/// </summary>
	~ServerManager();
	/// <summary>
	/// map of rooms by their room code
	/// </summary>
	std::map<std::string, RoomInfo> rooms;
	/// <summary>
	/// singleton instance to the servermanager 
	/// </summary>
	static ServerManager* instance;
	/// <summary>
	/// the data of all the connected clients
	/// </summary>
	vector<ClientInfo*> clientsData;
	/// <summary>
	/// the number of connected clients
	/// </summary>
	size_t connectedClientsCount;
	/// <summary>
	/// a pointer to the server
	/// </summary>
	Server* server;
	/// <summary>
	/// the context to run the python moudule for the firebase
	/// </summary>
	asio::io_context pythonContext;
	/// <summary>
	/// the pybind firebaes module written in python
	/// </summary>
	pybind11::module_ fireBaseModule;
	/// <summary>
	/// asio work to keep the python context alive
	/// </summary>
	asio::io_context::work* pythonWork;
};

