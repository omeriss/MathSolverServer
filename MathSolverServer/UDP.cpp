#include "UDP.h"

UDP::UDP(asio::io_service& serv, asio::ip::udp::socket* sock, asio::ip::udp::endpoint endpoint, uint32_t id) :service(serv), SendStrand(serv)
{
	this->endpoint = endpoint;
	this->id = id;
	socket = sock;
}

UDP::~UDP()
{
}

asio::ip::udp::socket& UDP::GetSocket()
{
	return *socket;
}


void UDP::ReadPacket()
{
	Packet* temp = new Packet();
	vector<asio::mutable_buffers_1> tempVec = { asio::buffer(&temp->GetHeader(), sizeof(Header)), asio::buffer(buffer) };
	socket->async_receive_from(tempVec,
		endpoint,
		[temp, this](std::error_code errorCode, int len) {
			if (errorCode) {
				delete temp;
			}
			else {
				len -= sizeof(Header);
				temp->Resize(len);
				std::memcpy(temp->GetData(), buffer.data(), len);
				PacketExecutor::GetInstance()->HandlePacket(id, temp);
				ReadPacket();
			}
		});
}

void UDP::Send(Packet& packet)
{
	service.post(SendStrand.wrap([this, packet]() {
		bool notWriting = sendQueue.empty();
		sendQueue.push(packet);
		if (notWriting) {
			Write();
		}
		}));
}

void UDP::SetId(uint32_t id)
{
	this->id = id;
}

void UDP::Disconnect()
{
	socket->close();
}

uint32_t UDP::GetId()
{
	return id;
}

void UDP::Write()
{
	if (sendQueue.empty())
		return;
	Packet& tempPacket = sendQueue.front();
	vector<asio::mutable_buffers_1> tempVec = { asio::buffer(&tempPacket.GetHeader(), sizeof(Header)) };
	if (tempPacket.GetHeader().size)
		tempVec.push_back(asio::buffer(tempPacket.GetData(), tempPacket.GetHeader().size));
	socket->async_send_to(tempVec, endpoint, [this](std::error_code errorCode, int len) {
		if (errorCode) {
			std::cout << "cant Write" << std::endl;
		}
		else {
			sendQueue.pop();
			Write(); // check
		}
		});
}
