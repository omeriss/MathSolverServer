#include "ServerManager.h"

ServerManager* ServerManager::GetInstance()
{
    if (instance == NULL)
        ServerManager::instance = new ServerManager();
    return instance;
}

void ServerManager::Start(int16_t port, size_t maxClients)
{
    clientsData.resize(maxClients);
    server = new Server(maxClients);
    server->StartServer(port);
}

Server* ServerManager::GetServer()
{
    return server;
}

void ServerManager::Disconnect(uint32_t id, bool remove)
{
    if(remove && clientsData[id]) {
        std::string roomCode = clientsData[id]->roomCode;
        auto roomIterator = rooms.find(roomCode);
        if (roomIterator != rooms.end()) {
            std::shared_ptr<Packet> packet = std::make_shared<Packet>();
            packet->GetHeader().packetType = UserDisconnect;
            (*packet) << id;
            SendToRoomOf(id, packet);
            auto clientIterator = roomIterator->second.clients.begin();
            for (; clientIterator != roomIterator->second.clients.end(); clientIterator++) {
                if ((*clientIterator)->id == id)
                    break;
            }
            if (clientIterator != roomIterator->second.clients.end())
                roomIterator->second.clients.erase(clientIterator);
        }
        delete clientsData[id];
        clientsData[id] = nullptr;
        std::cout << "[" << id << "] " << "Disconnected" << std::endl;
        server->Disconnect(id);
    }
    else {
        server->Disconnect(id, clientsData[id] == NULL);
    }
}

void ServerManager::AddClient(uint32_t id)
{
    clientsData[id] = new ClientInfo();
    clientsData[id]->id = id;
}

std::string ServerManager::CreateRoom(uint32_t host)
{
    std::string roomCode;
    int charCount = RoomCodeCharCount;
    do {
        roomCode = "";
        for (int i = 0; i < RoomCodeLen; i++) {
            int rnd = rand() % charCount;
            roomCode += RoomCodeChars[rnd];
        }
    } while (rooms.find(roomCode) != rooms.end());

    rooms[roomCode] = RoomInfo();
    rooms[roomCode].host = clientsData[host];
    rooms[roomCode].clients.push_back(clientsData[host]);
    clientsData[host]->roomCode = roomCode;


    std::cout << "[server] Created room: " << roomCode << std::endl;
    std::cout << "[" << host << "] Joined room "<< roomCode << " as a host"<< std::endl;

    return roomCode;
}

bool ServerManager::AddToRoom(std::string roomCode, uint32_t id)
{
    auto cit = rooms.find(roomCode);
    if (cit != rooms.end()) {
        for (auto client : cit->second.clients) {
            std::shared_ptr<Packet> packet = std::make_shared<Packet>();
            packet->GetHeader().packetType = UserConnected;
            (*packet) << client->id;
            server->SendTcp(packet, id);
        }
        clientsData[id]->roomCode = roomCode;
        cit->second.clients.push_back(clientsData[id]);
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        packet->GetHeader().packetType = UserConnected;
        (*packet) << id;
        SendToRoomOf(id, packet);
        std::cout << "[" << id << "] Joined room " << roomCode << std::endl;
        return true;
    }
    return false;
}

void ServerManager::SendToRoomOf(uint32_t id, std::shared_ptr<Packet> packet, int protocol)
{
    if (!clientsData[id])
        return;
    auto cit = rooms.find(clientsData[id]->roomCode);
    if (cit == rooms.end())
        return;
    for (auto client : cit->second.clients) {
        if (client->id != id) {
            if (protocol == 0)
                server->SendTcp(packet, client->id);
            else if (protocol == 1)
                server->SendUdp(packet, client->id);
        }
    }

}

void ServerManager::SendFromTo(uint32_t from, uint32_t to, std::shared_ptr<Packet> packet, int protocol)
{
    if (!clientsData[from] || !clientsData[to])
        return;
    if (clientsData[from]->roomCode != clientsData[to]->roomCode)
        return;
    auto cit = rooms.find(clientsData[from]->roomCode);
    if (cit == rooms.end())
        return;
    if (protocol == 0)
        server->SendTcp(packet, to);
    else if (protocol == 1)
        server->SendUdp(packet, to);

}