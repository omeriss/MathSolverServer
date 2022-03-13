#pragma once
#include <vector>
#include <memory>
#include <iostream>
using std::vector;

typedef unsigned char Byte;
typedef int32_t EnumType;

enum PacketsServer :EnumType {
	DisconnectFromServer, Ping, Audiop
};

enum PacketsClient :EnumType {
	OnClientDisconnect, PingBack, Audios
};

struct Header {
	EnumType packetType;
	__int32 size = 0;
};

class Packet
{
public:
	Packet();
	Header& GetHeader();
	int GetSize();
	void Resize(int len);
	Byte* GetData();
	template<typename InsertType>
	friend Packet& operator <<(Packet& packet, InsertType insertData) {
		packet.header.size = packet.data.size() + sizeof(InsertType);
		packet.data.resize(packet.header.size);
		std::memcpy(packet.data.data() + (packet.header.size - sizeof(InsertType)), &insertData, sizeof(InsertType));
		return packet;
	}
	friend Packet& operator <<(Packet& packet, std::string insertData) {
		packet.header.size = packet.data.size() + insertData.length() + 1;
		packet.data.resize(packet.header.size);
		std::memcpy(packet.data.data() + (packet.header.size - (insertData.length() + 1)), insertData.c_str(), insertData.length() + 1);
		return packet;
	}
	template<typename readType>
	friend Packet& operator >>(Packet& packet, readType& ReadData) {
		std::memcpy(&ReadData, packet.data.data() + packet.readFrom, sizeof(readType));
		packet.readFrom += sizeof(readType);
		return packet;
	}
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
	Header header;
	vector<Byte> data;
	int readFrom;

};

