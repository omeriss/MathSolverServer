#include "UDP.h"

UDP::UDP(asio::io_service& serv, asio::ip::udp::endpoint endpoint, uint32_t id) :NetworkProtocol(serv)
{
	std::string addr = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
	UDP::readData.clientsMap.Set(addr, id);
	this->endpoint = endpoint;
	this->id = id;
}

UDP::~UDP()
{
	std::string addr = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
	UDP::readData.clientsMap.Remove(addr);
}


void UDP::ReadPacket()
{
	Packet* temp = new Packet();
	vector<asio::mutable_buffers_1> tempVec = { asio::buffer(&temp->GetHeader(), sizeof(Header)), asio::buffer(readData.buffer) };
	socket->async_receive_from(tempVec,
		readData.reciveEndPoint,
		[temp](std::error_code errorCode, int len) {
			if (errorCode) {
				delete temp;
				if (errorCode.value() != 995) {
					ReadPacket();
				}
				else {
					std::cout << "stopping udp";
				}
			}
			else {
				std::string addr = readData.reciveEndPoint.address().to_string() + ":" + std::to_string(readData.reciveEndPoint.port());
				uint32_t id = readData.clientsMap.Get(addr);
				if (id == UINT32_MAX) {
					delete temp;
					ReadPacket();
					return;
				}
				len -= sizeof(Header);
				temp->Resize(len);
				std::memcpy(temp->GetData(), readData.buffer.data(), len);
				PacketExecutor::GetInstance()->HandlePacket(id, temp);
				ReadPacket();
			}
		});
}

asio::ip::udp::endpoint& UDP::GetEndPint()
{
	return endpoint;
}

void UDP::Write()
{
	if (sendQueue.empty())
		return;
	std::shared_ptr<Packet> tempPacket = sendQueue.front();
	vector<asio::mutable_buffers_1> tempVec = { asio::buffer(&tempPacket->GetHeader(), sizeof(Header)) };
	if (tempPacket->GetHeader().size)
		tempVec.push_back(asio::buffer(tempPacket->GetData(), tempPacket->GetHeader().size));
	socket->async_send_to(tempVec, endpoint, [this](std::error_code errorCode, int len) {
		if (errorCode) {
			std::cout << "cant Write" << std::endl;
			sendQueue.pop();
			Write(); // check
		}
		else {
			sendQueue.pop();
			Write(); // check
		}
		});
}
