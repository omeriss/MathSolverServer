#include "Packet.h"

Packet::Packet()
{
	readFrom = 0;
}

Header& Packet::GetHeader()
{
	return header;
}

int Packet::GetSize()
{
	return sizeof(header) + data.size();
}

void Packet::Resize(int len)
{
	data.resize(len);
}

Byte* Packet::GetData()
{
	return data.data();
}
