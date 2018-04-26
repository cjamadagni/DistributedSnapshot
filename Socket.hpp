/*
 * Socket.hpp
 * This file is part of VallauriSoft
 *
 * Copyright (C) 2012 - Comina, gnuze
 *
 * VallauriSoft is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * VallauriSoft is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VallauriSoft. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <stdlib.h>

#include <arpa/inet.h>

#define MAX_BUFFER 1024

using namespace std;

namespace Socket
{
    typedef struct
    {
        string ip;
        int port;
    } Address;

    typedef struct
    {
        Address address;
        string data;
    } Datagram;

    class Exception
    {
    private:
        string _message;
    public:
        Exception(string error) { this->_message = error; }
        virtual const char* what() { return this->_message.c_str(); }
    };

    class UDP
    {
    private:

        int id;
        bool isBinded;

    public:

        UDP(void);
        ~UDP(void);
        void close(void);
        void bind(int port);
        void send(string ip, int port, string data);
        Datagram receive();
    };

    UDP::UDP(void)
        {
            this->id = socket(AF_INET, SOCK_DGRAM, 0);
            if (this->id == -1) throw Exception("[Constructor] Cannot create socket");
            this->isBinded = false;
        }

    UDP::~UDP(void)
        {
        }

    void UDP::close(void)
        {
            shutdown(this->id, SHUT_RDWR);
        }

    void UDP::bind(int port)
        {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr=htonl(INADDR_ANY);
            address.sin_port=htons(port);

            if (this->isBinded)
            {
                this->close();
                this->id = socket(AF_INET, SOCK_DGRAM, 0);
            }
            // ::bind() calls the function bind() from <arpa/inet.h> (outside the namespace)
            if (::bind(this->id, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
            {
                stringstream error;
                error << "[listen_on_port] with [port=" << port << "] Cannot bind socket";
                throw Exception(error.str());
            }

            this->isBinded = true;
        }

    void UDP::send(string ip, int port, string data)
        {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_port = htons(port);
            inet_aton(ip.c_str(), &address.sin_addr);

            if (sendto(this->id, (void*)data.c_str(), data.length() + 1, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
            {
                stringstream error;
                error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << data << "] Cannot send";
                throw Exception(error.str());
            }
        }

    Datagram UDP::receive()
        {
            int size = sizeof(struct sockaddr_in);
            char *buffer = (char*)malloc(sizeof(char) * MAX_BUFFER);
            struct sockaddr_in address;
            Datagram ret;

            if (recvfrom(this->id, (void*)buffer, MAX_BUFFER, 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("[receive] Cannot receive");

            ret.data = buffer;
            ret.address.ip = inet_ntoa(address.sin_addr);
            ret.address.port = ntohs(address.sin_port);

            free(buffer);

            return ret;
        }
}

#endif   // _SOCKET_HPP_
