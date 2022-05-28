// MathSolverServer.cpp : This file contains the 'main' function. Program execution begins and ends there.

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include "Server.h"
#include "HandlePacketFunctions.h"
#include <Windows.h>

PacketExecutor* PacketExecutor::instance = NULL;
ServerManager* ServerManager::instance = NULL;
asio::ip::udp::socket* UDP::socket = NULL;
UdpReadData UDP::readData;

BOOL WINAPI ConsoleCtrlHandler(DWORD ctrlType)
{
    ServerManager::GetInstance()->GetServer()->Kill();
    switch (ctrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        ServerManager::DelInstance();
    default:
        return FALSE;
    }
}

/// <summary>
/// read the config file
/// </summary>
/// <param name="confingName"> the path to the config files</param>
/// <returns> a map of the configss</returns>
std::map<std::string, std::string> ReadConfig(std::string confingName) {
    std::map<std::string, std::string> config;
    std::ifstream file;
    file.open(confingName);
    if (!file) {
        std::cout << "cant open config";
        return {};
    }
    std::string line;
    while (std::getline(file, line)) {
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        int eq = line.find("=");
        if (eq == std::string::npos)
            continue;
        config[line.substr(0, eq)] = line.substr(eq + 1, line.size()-eq-1);
    }
    return config;
}

int main()
{
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
    srand(time(NULL));

    // read configs
    auto config = ReadConfig(Config_File_Name);

    // get port
    int16_t port;
    try {
        port = (config.find("port") != config.end()) ? stoi(config["port"]) : Default_Port;
    }
    catch (...) {
        port = Default_Port;
    }

    // get max Clients
    int16_t maxClients;
    try {
        maxClients = (config.find("max_clients") != config.end()) ? stoi(config["max_clients"]) : Default_Max_Clinets;
    }
    catch (...) {
        int16_t maxClients = Default_Max_Clinets; 
    }

    // get userName
    std::string email;
    if(config.find("email") != config.end()) {
        email = config["email"];
    }
    else {
        std::cout << "enter email:" << std::endl;
        std::cin >> email;
    }

    // get password
    std::string password;
    if (config.find("password") != config.end()) {
        password = config["password"];
    }
    else {
        std::cout << "enter password:" << std::endl;
        std::cin >> password;
    }

    // create the packet executor
    PacketExecutor::GetInstance()->SetFunctions(CreatePacketVector);

    // start the server
    std::cout << "Server starting on port: " << port << std::endl;
    ServerManager::GetInstance()->Start(16016, 30, email, password);
    
    // run server
    while (true) {
        PacketExecutor::GetInstance()->Udate();
    }
}
