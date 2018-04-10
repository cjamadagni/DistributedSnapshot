
#include <iostream>
#include "Socket.hpp"

#define CLIENT_PORT 3000
#define SERVER_PORT 2000

int main() {
  try {
    Socket::UDP s;
    s.bind(2000);

    Socket::Datagram d = s.receive();

    if (d.data == "S") {
      s.send("127.0.0.1", CLIENT_PORT, "Successful Checkpoint");
      s.close();
    }
  }
  catch(Socket::Exception &e) {
    std::cout << e.what() << endl << endl;
  }
  return 1;
}
