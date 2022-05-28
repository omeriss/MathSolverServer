#pragma once
#include "NetworkProtocol.h"

class TCP
	:public NetworkProtocol
{
public:
	/// <summary>
	/// constructor
	/// </summary>
	/// <param name="service"> the service - needer for async funcs</param>
	TCP(asio::io_service& service);

	/// <summary>
	/// get the tcp socket
	/// </summary>
	/// <returns> the socket</returns>
	asio::ip::tcp::socket& GetSocket();

	~TCP();

	/// <summary>
	/// read packet from connection
	/// </summary>
	void ReadPacket();

	/// <summary>
	/// stop the connection
	/// </summary>
	void Disconnect();
private:
	/// <summary>
	/// write the packet
	/// </summary>
	void Write();
	asio::ip::tcp::socket socket;
};

