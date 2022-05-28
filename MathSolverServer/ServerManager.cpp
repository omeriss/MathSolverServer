#include "ServerManager.h"

ServerManager* ServerManager::GetInstance()
{
    if (instance == NULL)
        ServerManager::instance = new ServerManager();
    return instance;
}

void ServerManager::DelInstance()
{
    ServerManager* temp = ServerManager::instance;
    instance = nullptr;
    if (temp != NULL)
        delete temp;
}

void ServerManager::Start(int16_t port, size_t maxClients, std::string email, std::string password)
{
    clientsData.resize(maxClients);
    server = new Server(maxClients);
    server->StartServer(port);

    pythonWork = new asio::io_context::work(pythonContext);
    pybind11::gil_scoped_release release();
    std::thread* pythonThread = new std::thread([this]() {
        pybind11::gil_scoped_acquire acquire();
        pybind11::scoped_interpreter gard{};
        pythonContext.run();
        });
    pythonThread->detach();

    pythonContext.post([this]() {
        this->fireBaseModule = pybind11::module_::import("fireBaseModule");
    });

    pythonContext.post([this]() {
        auto initFireBase = fireBaseModule.attr("init_firebase");
        initFireBase();
    });

    pythonContext.post([this, email, password, port, maxClients]() {
        auto log_in = fireBaseModule.attr("log_in");
        bool connected = pybind11::cast<bool>(log_in(email.c_str(), password.c_str()));
        if (connected) {
            std::cout << "connected to public database" << std::endl;
            pythonContext.post([this, port, maxClients]() {
                auto set_server = fireBaseModule.attr("set_server");
                set_server(port, maxClients);
            });
        }
        else {
            std::cout << "can't connect to public database" << std::endl;
        }
    });
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

            if (clientIterator != roomIterator->second.clients.end()) {
                if ((*clientIterator)->participentType == ParticipentType::Host) {
                    if (roomIterator->second.clients.size() == 1) {
                        rooms.erase(roomIterator);
                    }
                    else {
                        if (roomIterator->second.clients[0]->id != id) {
                            SetParticipentType(roomIterator->second.clients[0]->id, ParticipentType::Host);
                        }
                        else {
                            SetParticipentType(roomIterator->second.clients[1]->id, ParticipentType::Host);
                        }
                        roomIterator->second.clients.erase(clientIterator);
                    }
                }
                else
                    roomIterator->second.clients.erase(clientIterator);
            }
        }
        delete clientsData[id];
        clientsData[id] = nullptr;
        std::cout << "[" << id << "] " << "Disconnected" << std::endl;
        server->Disconnect(id);

        connectedClientsCount--;
        pythonContext.post([this]() {
            auto change_user_count = fireBaseModule.attr("change_user_count");
            change_user_count(this->connectedClientsCount);
        });
    }
    else {
        server->Disconnect(id, clientsData[id] == NULL);
    }
}

void ServerManager::AddClient(uint32_t id, std::string& userName)
{
    clientsData[id] = new ClientInfo();
    clientsData[id]->id = id;
    clientsData[id]->userName = userName;

    connectedClientsCount++;
    pythonContext.post([this]() {
        auto change_user_count = fireBaseModule.attr("change_user_count");
        change_user_count(this->connectedClientsCount);
    });
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
    //SetParticipentType(host, ParticipentType::Host);

    std::cout << "[server] Created room: " << roomCode << std::endl;
    std::cout << "[" << host << "] Joined room "<< roomCode << " as a host"<< std::endl;

    return roomCode;
}

bool ServerManager::AddToRoom(std::string roomCode, uint32_t id)
{
    if (!clientsData[id])
        return false;
    auto cit = rooms.find(roomCode);
    if (cit != rooms.end()) {
        //for (auto client : cit->second.clients) {
        //    std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        //    packet->GetHeader().packetType = UserConnected;
        //    (*packet) << client->id;
        //    (*packet) << client->participentType;
        //    server->SendTcp(packet, id);
        //}
        clientsData[id]->roomCode = roomCode;
        cit->second.clients.push_back(clientsData[id]);
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        packet->GetHeader().packetType = UserConnected;
        (*packet) << id;
        (*packet) << clientsData[id]->participentType;
        (*packet) << clientsData[id]->userName;
        SendToRoomOf(id, packet);
        std::cout << "[" << id << "] Joined room " << roomCode << std::endl;
        return true;
    }
    return false;
}

void ServerManager::SendRoomClientsToConnectedUser(uint32_t id)
{
    if (!clientsData[id])
        return;
    auto cit = rooms.find(clientsData[id]->roomCode);
    if (cit != rooms.end()) {
        for (auto client : cit->second.clients) {
            if (client->id != id) {
                std::shared_ptr<Packet> packet = std::make_shared<Packet>();
                packet->GetHeader().packetType = UserConnected;
                (*packet) << client->id;
                (*packet) << client->participentType;
                (*packet) << client->userName;
                server->SendTcp(packet, id);
            }
        }
    }
}

void ServerManager::SendToRoomOf(uint32_t id, std::shared_ptr<Packet> packet, int protocol, bool include)
{
    if (!clientsData[id])
        return;
    auto cit = rooms.find(clientsData[id]->roomCode);
    if (cit == rooms.end())
        return;
    for (auto client : cit->second.clients) {
        if (client->id != id || include) {
            if (protocol == 0)
                server->SendTcp(packet, client->id);
            else if (protocol == 1 && clientsData[client->id] && clientsData[client->id]->UdpOk)
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
    else if (protocol == 1 && clientsData[to]->UdpOk)
        server->SendUdp(packet, to);

}

void ServerManager::ActivateUdpOk(uint32_t id)
{
    if (!clientsData[id])
        return;
    clientsData[id]->UdpOk = true;
}

ParticipentType ServerManager::GetParticipentType(uint32_t id)
{
    if (!clientsData[id])
        return ParticipentType::WatchOnly;
    return clientsData[id]->participentType;
}

void ServerManager::SetParticipentType(uint32_t id, ParticipentType participentType) {
    if (!clientsData[id])
        return;
    clientsData[id]->participentType = participentType;
    std::shared_ptr<Packet> packet = std::make_shared<Packet>();
    packet->GetHeader().packetType = ParticipentTypeChanged;
    (*packet) << id;
    (*packet) << participentType;
    SendToRoomOf(id, packet, 0, true);
}

ServerManager::ServerManager()
{
    this->connectedClientsCount = 0;
}

ServerManager::~ServerManager()
{
    std::cout << "exit" << std::endl;
    pythonContext.post([this]() {
        auto exit_fire_base = fireBaseModule.attr("exit_fire_base");
        exit_fire_base();
    });
    delete pythonWork;
    while (!pythonContext.stopped()) {}
}
