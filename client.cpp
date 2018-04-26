/*
Client code for a particular node.
@author Chirag Jamadagni
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Socket.hpp"

typedef struct {
  std::string ip;
  int port;
} Node;

// Given a node id, return the corresponding IP and port
Node getNodeInformation(std::vector<Node> &v, int id) {
  Node n;

  for (int i = 0; i < v.size(); i++) {
    if (v.at(i).port == id) {
      n.ip = v.at(i).ip;
      n.port = v.at(i).port;
      break;
    }
  }

  return n;
}

// Function to read config file and identify all node IDs
void initialize_clients(std::vector<Node> &v, int node_id) {
  std::string line;
  std::ifstream file;
  file.open("config.cfg");

  while (file && std::getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }

    int split_position = line.find(' ');
    int port = std::stoi(line.substr(split_position+1, line.length()));
    if (port != node_id) {
      Node x;
      x.port = port;
      x.ip = line.substr(0,split_position);
      v.push_back(x);
    }
  }
}

// Input validation to ensure that a valid node ID is entered for credit/debit operations
bool input_check(int node_id, std::vector<Node> &v) {
  bool flag = false;
  for (int i=0; i<v.size(); i++) {
    if (v.at(i).port == node_id) {
      flag = true;
    }
  }
  return flag;
}

// Utility function to print the option menu
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

  /*
  Reading command line argument to get server and client ports.
  NOTE: Node ID doubles up as the server port
  */
  int SERVER_PORT = std::stoi(argv[1]);
  int CLIENT_PORT = SERVER_PORT - 1;

  std::cout<<"\n\nNode ID = " << SERVER_PORT;

  // Initializing clients
  std::vector<Node> nodes;
  initialize_clients(nodes, SERVER_PORT);
  std::string num_of_nodes = std::to_string(nodes.size() + 1);


  try {

    // Creating and binding socket to port
    Socket::UDP s;
    s.bind(CLIENT_PORT);

    // Sending topology information current node's server
    s.send("127.0.0.1", SERVER_PORT, num_of_nodes);

    while (true) {

      choice = menu();

      // Trigger checkpoint / propogate snapshot markers
      if (choice == 1) {
        for (int i=0; i<nodes.size(); i++) {
          s.send(nodes.at(i).ip, nodes.at(i).port, "S");
        }
        s.send("127.0.0.1", SERVER_PORT, "S");
      }

      // Display the last checkpoint state
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

      // Credit / debit operations
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
        Node recipient = getNodeInformation(nodes, node_id);
        s.send(recipient.ip, recipient.port, out_msg);
        s.send("127.0.0.1", SERVER_PORT, in_msg);
      }

      // Powering off node / Used to simulate failure.
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
