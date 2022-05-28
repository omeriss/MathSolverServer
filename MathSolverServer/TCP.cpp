#include "TCP.h"

TCP::TCP(asio::io_service& s) :NetworkProtocol(s), socket(s)
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

	// read the header async 
	asio::async_read(socket, asio::buffer(&temp->GetHeader(), sizeof(Header)),
		[temp, this](std::error_code errorCode, int len) {
			if (errorCode) {
				Disconnect();
				Packet* disconnectPacket = new Packet();
				disconnectPacket->GetHeader().packetType = 0;
				PacketExecutor::GetInstance()->HandlePacket(id, disconnectPacket);
				delete temp;
			}
			else {

				// if there is no data don't try to read data
				if (temp->GetHeader().size) {
					temp->Resize(temp->GetHeader().size);

					// read the data async
					asio::async_read(socket, asio::buffer(temp->GetData(), temp->GetHeader().size),
						[temp, this](std::error_code errorCode, int len) {
							if (errorCode) {
								socket.close();
								Packet* disconnectPacket = new Packet();
								disconnectPacket->GetHeader().packetType = 0;
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

void TCP::Disconnect()
{
	try {
		if (socket.is_open())
			socket.close();
	}
	catch (...) {
		std::cout << "error" << std::endl;
	}
}

void TCP::Write()
{

	// take form the write queue
	if (sendQueue.empty())
		return;
	std::shared_ptr<Packet> tempPacket = sendQueue.front();
	vector<asio::mutable_buffers_1> tempVec = { asio::buffer(&tempPacket->GetHeader(), sizeof(Header)) };
	if (tempPacket->GetHeader().size)
		tempVec.push_back(asio::buffer(tempPacket->GetData(), tempPacket->GetHeader().size));

	// write both headder and data async
	asio::async_write(socket, tempVec, [this](std::error_code errorCode, int len) {
		if (errorCode) {
			std::cout << "cant Write" << std::endl;
		}
		else {
			sendQueue.pop();
			Write(); // check
		}
		});
}
