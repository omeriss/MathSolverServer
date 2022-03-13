#pragma once
#include "Server.h"

struct ClientInfo {
	bool isHost = false;
	bool isEditor = false;
};


class ServerManager
{
public:
	static ServerManager* GetInstance();
	void Start(int16_t port = 16016, size_t maxClients = 30);
	Server* GetServer();
	void Disconnect(uint32_t id);
	void AddClient(uint32_t id);
private:
	static ServerManager* instance;
	vector<ClientInfo*> clientsData;
	Server* server;
};

