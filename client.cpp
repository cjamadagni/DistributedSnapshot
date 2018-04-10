#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Socket.hpp"

void initialize_clients(std::vector<int> &v, int my_port) {
  std::string line;
  std::ifstream file;
  file.open("config.cfg");
  while (file && std::getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }
    int port = std::stoi(line);
    if (port != my_port) {
        v.push_back(port);
    }
  }
  file.close();
}

bool input_check(int node_id, std::vector<int> &v) {
  bool flag = false;
  for (int i=0; i<v.size(); i++) {
    if (v.at(i) == node_id) {
      flag = true;
    }
  }
  return flag;
}

int menu() {
  int choice;
  std::cout << endl << endl;;
  std::cout << "1. Trigger Snapshot / Return Marker" << endl;
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

int main(int argc, char** argv) {
  int choice, node_id, amount;

  int SERVER_PORT = std::stoi(argv[1]);
  int CLIENT_PORT = SERVER_PORT - 1;

  std::cout<<"\n\nNode ID = " << SERVER_PORT;

  // Initializing clients
  std::vector<int> nodes;
  initialize_clients(nodes, SERVER_PORT);


  try {

    // Creating and binding socket to port
    Socket::UDP s;
    s.bind(CLIENT_PORT);

    while (true) {

      choice = menu();

      if (choice == 1) {
        for (int i=0; i<nodes.size(); i++) {
          s.send("127.0.0.1", nodes.at(i), "S");
        }
        s.send("127.0.0.1", SERVER_PORT, "S");
        //Socket::Datagram d = s.receive();
        //std::cout << d.data << endl;
      }

      else if (choice == 2) {
        s.send("127.0.0.1", SERVER_PORT, "L");
        Socket::Datagram d1 = s.receive();
        std::cout << endl << d1.data << endl << endl;
        Socket::Datagram d2 = s.receive();
        if (d2.data.length() > 0) {
          std::cout << "Staged Transations" << endl;
          std::cout << d2.data << endl;
        }
      }

      else if (choice == 3 || choice == 4) {
        std::cout << endl << endl;
        std::cout << "Enter node ID to credit/debit: ";
        std::cin >> node_id;
        std::cout << endl << "Enter Amount: $";
        std::cin >> amount;
        std::cout << endl << endl;

        if (!input_check(node_id,nodes)) {
          std::cout << "Entered Node ID not in the config file" << endl << endl;
          continue;
        }

        std::string out_msg, in_msg;
        if (choice == 3) {
          out_msg = "C " + std::to_string(amount);
          in_msg = "D " + std::to_string(amount);
        }
        else if (choice == 4) {
          out_msg = "D " + std::to_string(amount);
          in_msg = "C " + std::to_string(amount);
        }
        s.send("127.0.0.1", node_id, out_msg);
        s.send("127.0.0.1", SERVER_PORT, in_msg);
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
