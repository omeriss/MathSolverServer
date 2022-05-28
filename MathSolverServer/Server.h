#pragma once
#include "TCP.h"
#include "UDP.h"


class Server
{
public:
	/// <summary>
	/// Constreactor for the server class
	/// </summary>
	/// <param name="maxClients">The max amout of clients that can be connected to the server</param>
	Server(size_t maxClients);
	/// <summary>
	/// start the server
	/// </summary>
	/// <param name="port"> the port the server will start on</param>
	void StartServer(int16_t port);
	/// <summary>
	/// This function is used to accept a clint connection and add it to the server
	/// </summary>
	/// <param name="sptr">a smart pointer to the client objest the the new cliet will be connected to</param>
	/// <param name="errorCode">error code</param>
	void ConnectClient(std::shared_ptr<TCP> sptr, asio::error_code& errorCode);
	/// <summary>
	/// send a udp packet to the server
	/// </summary>
	/// <param name="packet">A smart ptr to the packet object</param>
	/// <param name="id">the id of the user we send the packet to</param>
	void SendUdp(std::shared_ptr<Packet> packet, uint32_t id);
	/// <summary>
	/// send all client a packet using udp
	/// </summary>
	/// <param name="packet">A smart pointer to the packet we want to send</param>
	/// <param name="exp"> if changed, the function will send the packet to all 
	/// client but the client with this id</param>
	void SendAllUdp(std::shared_ptr<Packet> packet, uint32_t exp = UINT32_MAX);
	/// <summary>
	/// send a tcp packet to the server
	/// </summary>
	/// <param name="packet">A smart ptr to the packet object</param>
	/// <param name="id">the id of the user we send the packet to</param>
	void SendTcp(std::shared_ptr<Packet> packet, uint32_t id);
	/// <summary>
	/// send all client a packet using udp
	/// </summary>
	/// <param name="packet">A smart pointer to the packet we want to send</param>
	/// <param name="exp"> if changed, the function will send the packet to all 
	/// client but the client with this id</param>
	void SendAllTcp(std::shared_ptr<Packet> packet, uint32_t exp = UINT32_MAX);
	/// <summary>
	/// Disconnet a client from the server
	/// </summary>
	/// <param name="id"> the id of the clinet we want to disconnect</param>
	/// <param name="remove"> if true, the client will also be removed from the clinets vector</param>
	void Disconnect(uint32_t id, bool remove = true);
	/// <summary>
	/// stops the server
	/// </summary>
	void Kill();


private:
	/// <summary>
	/// the io service to run async
	/// </summary>
	asio::io_service service;

	/// <summary>
	/// the thread to run the io context
	/// </summary>
	std::thread serviceThread;

	/// <summary>
	/// lisenner
	/// </summary>
	asio::ip::tcp::acceptor acceptor;

	/// <summary>
	/// the connections to the clients
	/// </summary>
	vector<std::pair<std::shared_ptr<TCP>, std::shared_ptr<UDP>>> connections;
};

