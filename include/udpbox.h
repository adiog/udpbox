// This file is a part of udpbox project.
// Copyright 2018 Aleksander Gajewski <adiog@brainfuck.pl>.

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32

#include <unistd.h>
#include <functional>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#else
#include <udpbox-win.h>
#endif

#include <string>
#include <thread>
#include <vector>

#define MAX_UDP_PACKET_SIZE 65535


namespace udpbox {

struct Datagram
{
    Datagram(const uint8_t *payload, int payloadLength, sockaddr_in senderAddress)

            : sender(inet_ntoa(senderAddress.sin_addr))
            , payload(payload, payload + payloadLength)
    {
    }

    std::string sender;
    std::vector<uint8_t> payload;
};

using OnDatagramCallback = std::function<void(udpbox::Datagram &&)>;

struct Server
{
    Server(int port)
            : port(port)
    {
    }

    ~Server()
    {
        stop();
    }

    void setOnDatagramCallback(OnDatagramCallback onDatagramCallback)
    {
        this->onDatagramCallback = onDatagramCallback;
    }

    void start()
    {
        isRunning = true;
        listener = std::thread([this]() { synchronousListenerThread(); });
    }

    void stop()
    {
        if (isRunning)
        {
            isRunning = false;
            if (listener.joinable())
            {
                listener.join();
            }
        }
    }

private:
    void synchronousListenerThread()
    {
#ifndef _WIN32
      int socketFileDescriptor;
        uint8_t buffer[MAX_UDP_PACKET_SIZE];
        sockaddr_in serverAddress;
        sockaddr_in clientAddress;
        socklen_t socketLength;

        if ((socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            perror("udpbox: creating socket failed");
            exit(EXIT_FAILURE);
        }

        memset(&serverAddress, 0, sizeof(serverAddress));
        memset(&clientAddress, 0, sizeof(clientAddress));

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        timeval receivingTimeout;
        receivingTimeout.tv_sec = 0;
        receivingTimeout.tv_usec = 100000;
        if (setsockopt(socketFileDescriptor, SOL_SOCKET, SO_RCVTIMEO, &receivingTimeout, sizeof(receivingTimeout)) < 0)
        {
            perror("udpbox: setting timeout failed");
            exit(EXIT_FAILURE);
        }

        if (bind(socketFileDescriptor, reinterpret_cast<const sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0)
        {
            perror("udpbox: binding socket address failed");
            exit(EXIT_FAILURE);
        }

        while (isRunning)
        {
            int datagramSize = recvfrom(socketFileDescriptor, buffer, MAX_UDP_PACKET_SIZE, MSG_WAITALL, reinterpret_cast<sockaddr *>(&clientAddress), &socketLength);
            if (datagramSize != -1)
            {
                Datagram datagram(buffer, datagramSize, clientAddress);
                onDatagramCallback(std::move(datagram));
            }
        }
#else
            try
    {
        WSASession Session;
        UDPSocket Socket;
        char buffer[MAX_UDP_PACKET_SIZE];
int datagramSize;
        Socket.Bind(port);
        while (isRunning)
        {
            sockaddr_in clientAddress = Socket.RecvFrom(buffer, sizeof(buffer), &datagramSize);
                Datagram datagram(reinterpret_cast<uint8_t *>(buffer), datagramSize, clientAddress);
                onDatagramCallback(std::move(datagram));
        }
    }
    catch (std::system_error& e)
    {
        std::cout << e.what();
    }
#endif
    }

    int port;
    bool isRunning;
    OnDatagramCallback onDatagramCallback;
    std::thread listener;
};
}
