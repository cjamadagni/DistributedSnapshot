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

typedef struct {
  string ip;
  int port;
} Address;

typedef struct {
  Address address;
  string data;
} Datagram;

class Exception {
  private:
    string exception_msg;

  public:
    Exception(string error);
    ~Exception();
    string exp();
};

class UDP {

  // defining private members
  private:
    int id;
    bool isBinded;

  // defining public members
  public:
    UDP(); // constructor
    ~UDP(); // destructor

    void close(); // closing the socket
    void bind(int port); // binding socket to the port
    void send(string ip, int port, string data); // sending datagram
    Datagram receive(); // receiving datagram
};

#endif //SOCKET_WRAPPER_H
