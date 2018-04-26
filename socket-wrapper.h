/*
  Lightweight wrapper for UDP sockets
  @author Chirag Jamadagni
*/

#ifndef SOCKET_WRAPPER_H
#define SOCKET_WRAPPER_H

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <stdlib.h>
#include <arpa/inet.h>

using namespace std;

// defining constants
#define MAX_BUFFER 1024

// custom type for IP address and port
typedef struct {
  string ip;
  int port;
} Address;

// custom type for data packets
typedef struct {
  Address address;
  string data;
} Packet;

// Class for exception handling
class Exception {
  private:
    string exception_msg;

  public:
    Exception(string error);
    ~Exception();
    string exp();
};

// Class for socket wrapper 
class Socket {
  // defining private members
  private:
    int id;
    bool isBinded;

  // defining public members
  public:
    Socket(); // constructor
    ~Socket(); // destructor

    void close(); // closing the socket
    void bind(int port); // binding socket to the port
    void send(string ip, int port, string data); // sending Packet
    Packet receive(); // receiving Packet
};

#endif //SOCKET_WRAPPER_H
