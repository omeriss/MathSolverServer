#include "Server.h"

Server::Server(size_t maxClients):acceptor(service)
{
	connections.resize(maxClients);
}

void Server::StartServer(int16_t port)
{
	udpSocket = new asio::ip::udp::socket(service, asio::ip::udp::endpoint(asio::ip::udp::v4(), port));

	auto sptr = std::make_shared<TCP>(service);
	asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
	acceptor.open(endpoint.protocol());

	acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
	acceptor.bind(endpoint);
	acceptor.listen();
	acceptor.async_accept(sptr->GetSocket(), [=](asio::error_code errorCode) {
		this->ConnectClient(sptr, errorCode);
		});

	serviceThread = std::thread([this]() { service.run(); });
}

void Server::ConnectClient(std::shared_ptr<TCP> sptr, asio::error_code& errorCode)
{
	if (!errorCode) {
		int i = 0;
		for (auto& con : connections) {
			if (!con.first || !con.first->GetSocket().is_open()) {
				sptr->SetId(i);
				con.first = std::move(sptr);
				std::cout << con.first->GetSocket().remote_endpoint().address() << "\n";
				con.second = std::make_shared<UDP>(
					service, udpSocket,
					asio::ip::udp::endpoint(con.first->GetSocket().remote_endpoint().address(),
					con.first->GetSocket().remote_endpoint().port()),
					i);
				con.first->ReadPacket();
				con.second->ReadPacket();
				std::cout << "[" << i << "] " << "Connected" << std::endl;
				break;
			}
			i++;
		}
		if (sptr) {
			std::cout << "Client Tried To Connect And Failed: Max Clients Connected" << std::endl;
		}
	}
	else {
		std::cout << "Client Tried To Connect And Failed:" << errorCode.message() << std::endl;
	}


	auto nextSptr = std::make_shared<TCP>(service);

	acceptor.async_accept(nextSptr->GetSocket(), [this, nextSptr](asio::error_code errorCode) {
		this->ConnectClient(nextSptr, errorCode);
		});

}

void Server::SendUdp(Packet& packet, uint32_t id)
{
	if(connections[id].second && connections[id].first && connections[id].first->GetSocket().is_open())
		connections[id].second->Send(packet);
}

void Server::SendAllUdp(Packet& packet, uint32_t exp)
{
	int i = 0;
	for (auto& con : connections) {
		if (con.first && con.second && con.first->GetSocket().is_open()) {
			con.second->Send(packet);
		}
		i++;
	}
}

void Server::SendTcp(Packet& packet, uint32_t id)
{
	if (connections[id].second && connections[id].first->GetSocket().is_open())
		connections[id].first->Send(packet);
}

void Server::SendAllTcp(Packet& packet, uint32_t exp)
{
	int i = 0;
	for (auto& con : connections) {
		if (con.second && con.first->GetSocket().is_open()) {
			con.first->Send(packet);
		}
		i++;
	}
}

void Server::Disconnect(uint32_t id)
{
	connections[id].first = NULL;
	connections[id].second = NULL;
}
