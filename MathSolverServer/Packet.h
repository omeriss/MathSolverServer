#pragma once
#include <vector>
#include <memory>
#include <iostream>
using std::vector;

typedef unsigned char Byte;
typedef int32_t EnumType;

// Packets from client to server
enum PacketsClient :EnumType {
	DisconnectFromServer, RoomRequest, Audiop, SendLine, SendCell, ChangeParticipentType
};

// packets from server to client
enum PacketsServer :EnumType {
	OnClientDisconnect, RoomResponse, Audios, SetLinePacket, SetCellPacket, UserConnected, UserDisconnect, ParticipentTypeChanged
};

struct Header {
	EnumType packetType;
	__int32 size = 0;
};

class Packet
{
public:
	/// <summary>
	/// constructor
	/// </summary>
	Packet();

	/// <summary>
	/// get the header
	/// </summary>
	/// <returns></returns>
	Header& GetHeader();

	/// <summary>
	/// get the size of data
	/// </summary>
	/// <returns> size of data</returns>
	int GetSize();

	/// <summary>
	/// change the size of data
	/// </summary>
	/// <param name="len">new size</param>
	void Resize(int len);

	/// <summary>
	/// get the data
	/// </summary>
	/// <returns> the data</returns>
	Byte* GetData();

	/// <summary>
	/// get data from read location
	/// </summary>
	/// <returns>the data from read location</returns>
	Byte* GetDataFromRead();

	/// <summary>
	/// write arr to the data
	/// </summary>
	/// <typeparam name="InsertType"> the data type</typeparam>
	/// <param name="insertData">the arr</param>
	/// <param name="len">the len</param>
	/// <returns></returns>
	template<typename InsertType>
	Packet& WriteArr(InsertType* insertData, size_t len) {
		header.size = data.size() + sizeof(InsertType) * len;
		data.resize(header.size);
		std::memcpy(data.data() + (header.size - sizeof(InsertType) * len), insertData, sizeof(InsertType) * len);
		return *this;
	}

	/// <summary>
	/// write data to data
	/// </summary>
	/// <typeparam name="InsertType"> the data type</typeparam>
	/// <param name="packet"> the packet</param>
	/// <param name="insertData"> the data</param>
	/// <returns> the packet</returns>
	template<typename InsertType>
	friend Packet& operator <<(Packet& packet, InsertType insertData) {
		packet.header.size = packet.data.size() + sizeof(InsertType);
		packet.data.resize(packet.header.size);
		std::memcpy(packet.data.data() + (packet.header.size - sizeof(InsertType)), &insertData, sizeof(InsertType));
		return packet;
	}

	/// <summary>
	/// write string to data
	/// </summary>
	/// <param name="packet"> the packet</param>
	/// <param name="insertData"> the string</param>
	/// <returns></returns>
	friend Packet& operator <<(Packet& packet, std::string insertData) {
		packet.header.size = packet.data.size() + insertData.length() + 1;
		packet.data.resize(packet.header.size);
		std::memcpy(packet.data.data() + (packet.header.size - (insertData.length() + 1)), insertData.c_str(), insertData.length() + 1);
		return packet;
	}

	/// <summary>
	/// read arr from data
	/// </summary>
	/// <typeparam name="readType"> the data type</typeparam>
	/// <param name="ReadData"> the arr</param>
	/// <param name="len"> the len of the arr</param>
	/// <returns></returns>
	template<typename readType>
	Packet& ReadArr(readType* ReadData, size_t len) {
		std::memcpy(ReadData, data.data() + readFrom, sizeof(readType) * len);
		readFrom += sizeof(readType) * len;
		return *this;
	}

	/// <summary>
	/// read data from the packet
	/// </summary>
	/// <typeparam name="readType"> the data type</typeparam>
	/// <param name="packet"> the packet</param>
	/// <param name="ReadData"> the data to read to </param>
	/// <returns></returns>
	template<typename readType>
	friend Packet& operator >>(Packet& packet, readType& ReadData) {
		std::memcpy(&ReadData, packet.data.data() + packet.readFrom, sizeof(readType));
		packet.readFrom += sizeof(readType);
		return packet;
	}

	/// <summary>
	/// read string from the packet
	/// </summary>
	/// <param name="packet"> the packet</param>
	/// <param name="ReadData"> the string to read to</param>
	/// <returns></returns>
	friend Packet& operator >>(Packet& packet, std::string& ReadData) {
		ReadData = "";
		int i = 0;
		while (packet.readFrom < packet.header.size && packet.data.data()[packet.readFrom]) {
			ReadData += packet.data.data()[packet.readFrom++];
		}
		packet.readFrom++;
		return packet;
	}

private:
	/// <summary>
	/// header
	/// </summary>
	Header header;

	/// <summary>
	/// the data in bytes
	/// </summary>
	vector<Byte> data;

	/// <summary>
	/// the point to where the data was already read
	/// </summary>
	int readFrom;

};

