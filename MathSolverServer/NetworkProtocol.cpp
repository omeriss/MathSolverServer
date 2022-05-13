#include "NetworkProtocol.h"


NetworkProtocol::NetworkProtocol(asio::io_service& service) :service(service), SendStrand(service)
{
}

void NetworkProtocol::Send(std::shared_ptr<Packet> packet)
{
	service.post(SendStrand.wrap([this, packet]() {
		bool notWriting = sendQueue.empty();
		sendQueue.push(packet);
		if (notWriting) {
			Write();
		}
		}));
}

void NetworkProtocol::SetId(uint32_t id)
{
	this->id = id;
}

uint32_t NetworkProtocol::GetId()
{
	return id;
}
