// MathSolverServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "Server.h"
#include "HandlePacketFunctions.h"

PacketExecutor* PacketExecutor::instance = NULL;
ServerManager* ServerManager::instance = NULL;
asio::ip::udp::socket* UDP::socket = NULL;
UdpReadData UDP::readData;

int main()
{
    sf::Thread t([]() {return; });
    PacketExecutor::GetInstance()->SetFunctions(CreatePacketVector);
    ServerManager::GetInstance()->Start(16016, 30);
    bool d = false;
    while (true) {
        PacketExecutor::GetInstance()->Udate();
    }
}
