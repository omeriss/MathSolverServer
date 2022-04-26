#include "TCP.h"

TCP::TCP(asio::io_service& s) :service(s), socket(s), SendStrand(s)
{
}

asio::ip::tcp::socket& TCP::GetSocket()
{
	return socket;
}

TCP::~TCP()
{
}

void TCP::ReadPacket()
{
	Packet* temp = new Packet();
	asio::async_read(socket, asio::buffer(&temp->GetHeader(), sizeof(Header)),
		[temp, this](std::error_code errorCode, int len) {
			if (errorCode) {
				socket.close();
				Packet* disconnectPacket = new Packet();
				disconnectPacket->GetHeader().packetType = 0;
				PacketExecutor::GetInstance()->HandlePacket(id, disconnectPacket);
				delete temp;
			}
			else {
				if (temp->GetHeader().size) {
					temp->Resize(temp->GetHeader().size);
					asio::async_read(socket, asio::buffer(temp->GetData(), temp->GetHeader().size),
						[temp, this](std::error_code errorCode, int len) {
						if (errorCode) {
							socket.close();
							Packet* disconnectPacket = new Packet();
							disconnectPacket->GetHeader().packetType = 0;
							socket.remote_endpoint().address();
							PacketExecutor::GetInstance()->HandlePacket(id, disconnectPacket);
							delete temp;
						}
						else {
							PacketExecutor::GetInstance()->HandlePacket(id, temp);
							ReadPacket();
						}
						});
				}
				else {
					PacketExecutor::GetInstance()->HandlePacket(id, temp);
					ReadPacket();
				}
			}
		});
}

void TCP::Send(std::shared_ptr<Packet> packet)
{
	service.post(SendStrand.wrap([this, packet]() {
		bool notWriting = sendQueue.empty();
		sendQueue.push(packet);
		if (notWriting) {
			Write();
		}
		}));
}

void TCP::SetId(uint32_t id)
{
	this->id = id;
}

uint32_t TCP::GetId()
{
	return id;
}


void TCP::Disconnect()
{
	if (socket.is_open())
		socket.close();
}

void TCP::Write()
{
	if (sendQueue.empty())
		return;
	std::shared_ptr<Packet> tempPacket = sendQueue.front();
	vector<asio::mutable_buffers_1> tempVec = { asio::buffer(&tempPacket->GetHeader(), sizeof(Header)) };
	if (tempPacket->GetHeader().size)
		tempVec.push_back(asio::buffer(tempPacket->GetData(), tempPacket->GetHeader().size));
	asio::async_write(socket, tempVec, [this](std::error_code errorCode, int len) { // needs to be wraped
		if (errorCode) {
			std::cout << "cant Write" << std::endl;
		}
		else {
			sendQueue.pop();
			Write(); // check
		}
		});
}
