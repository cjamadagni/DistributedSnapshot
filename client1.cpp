
#include <iostream>
#include "Socket.hpp"

#define CLIENT_PORT 3000
#define SERVER_PORT 2000

int menu() {

  int choice;

  std::cout << "\n\n";
  std::cout << "1. Trigger Snapshot\n";
  std::cout << "2. Show Last Saved State\n";
  std::cout << "\n\n";
  std::cout << "Choice: ";
  std::cin >> choice;

  if (choice < 0 || choice > 2) {
    std::cout << "\n\nInvalid choice\n\n";
  }

  return choice;

}

int main() {
  int choice;

  try {

    // Creating and binding socket to port
    Socket::UDP s;
    s.bind(CLIENT_PORT);

    while (true) {

      choice = menu();

      if (choice == 1) {
        s.send("127.0.0.1", SERVER_PORT, "S");
        Socket::Datagram d = s.receive();
        std::cout << d.data << endl;
      }

      else if (choice == 2) {
        s.close();
        break;
      }
    }
  }

  catch (Socket::Exception &e) {
    std::cout << e.what() << endl << endl;
  }
  return 1;
}
