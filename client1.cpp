
#include <iostream>
#include <string>
#include "Socket.hpp"

#define CLIENT_PORT 3000
#define SERVER_PORT 2000

int menu() {

  int choice;

  std::cout << endl << endl;;
  std::cout << "1. Trigger Snapshot" << endl;
  std::cout << "2. Show Last Saved State" << endl;
  std::cout << "3. Credit Amount" << endl;
  std::cout << "4. Debit Amount" << endl;
  std::cout << endl << endl;;
  std::cout << "Choice: ";
  std::cin >> choice;

  if (choice < 0 || choice > 4) {
    std::cout << "\n\nInvalid choice\n\n";
    return -1;
  }

  return choice;

}

int main() {
  int choice, node_id, amount;

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

      else if (choice == 3 || choice == 4) {
        std::cout << endl << endl;
        std::cout << "Enter node ID to credit/debit: ";
        std::cin >> node_id;
        std::cout << endl << "Enter Amount: $";
        std::cin >> amount;
        std::cout << endl << endl;

        std::string msg;
        if (choice == 3) {
          msg = "C " + std::to_string(amount);
        }
        else if (choice == 4) {
          msg = "D " + std::to_string(amount);
        }
        std::cout << msg;
        s.send("127.0.0.1", SERVER_PORT, msg);
      }

      else if (choice == 0) {
        s.send("127.0.0.1", SERVER_PORT, "T");
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
