// This file is a part of udpbox project.
// Copyright 2018 Aleksander Gajewski <adiog@brainfuck.pl>.

#include <udpbox.h>
#include <iostream>


int main()
{
    udpbox::Server server(1234);
    server.setOnDatagramCallback(
        [](udpbox::Datagram&& datagram) {
            std::string message(datagram.payload.cbegin(), datagram.payload.cend());
            std::cout << datagram.sender << ": [" << message << "]" << std::endl;
        });
    server.start();
    getchar();
    return 0;
}

