/*
  Lightweight socket wrapper
  @author Chirag Jamadagni
*/

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <stdlib.h>
#include <arpa/inet.h>
#include "socket-wrapper.h"

using namespace std;

// Exception class constructor
Exception::Exception(string m) {
  this->exception_msg = m;
}

// Exception class destructor
Exception::~Exception() {
}

// Function to return the raised exception
string Exception::exp() {
  return this->exception_msg;
}

// Socket class constructor
Socket::Socket() {
  this->id = socket(AF_INET, SOCK_DGRAM, 0);
  if (this->id == -1) throw Exception("Socket can't be created.");
  this->isBinded = false;
}

// Socket class destructor
Socket::~Socket() {
}

// Function to bind socket to a port
void Socket::bind(int port) {
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);

  if (this->isBinded) {
    this->close();
    this->id = socket(AF_INET, SOCK_DGRAM, 0);
  }

  if (::bind(this->id, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1) {
    stringstream error;
    error << "Cannot bind socket to port " << port;
    throw Exception(error.str());
  }

  this->isBinded = true;
}

// Function to send packet
void Socket::send(string ip, int port, string data) {
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  inet_aton(ip.c_str(), &address.sin_addr);

  if (sendto(this->id, (void*)data.c_str(), data.length() + 1, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1) {
    stringstream error;
    error << "Can't send data with ip = " << ip << " and port = " << port;
    throw Exception(error.str());
  }
}

// Function to receive packet
Packet Socket::receive() {
  int size = sizeof(struct sockaddr_in);
  char *buffer = (char*)malloc(sizeof(char) * MAX_BUFFER);
  struct sockaddr_in address;
  Packet ret;

  if (recvfrom(this->id, (void*)buffer, MAX_BUFFER, 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("Couldn't receive data.");

  ret.data = buffer;
  ret.address.ip = inet_ntoa(address.sin_addr);
  ret.address.port = ntohs(address.sin_port);

  free(buffer);

  return ret;
}

// Function for closing a socket
void Socket::close() {
  shutdown(this->id, SHUT_RDWR);
}
