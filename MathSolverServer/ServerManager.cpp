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

void ServerManager::Disconnect(uint32_t id)
{
    std::cout << "[" << id << "] " << "Disconnected" << std::endl;
    server->Disconnect(id);
}

void ServerManager::AddClient(uint32_t id)
{
    clientsData[id] = new ClientInfo();
}
