/*
Client code for a particular node.
@author Chirag Jamadagni
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include "Socket.hpp"
#include "socket-wrapper.h"

using namespace std;

typedef struct {
  string ip;
  int port;
} Node;

// Given a node id, return the corresponding IP and port
Node getNodeInformation(vector<Node> &v, int id) {
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
void initialize_clients(vector<Node> &v, int node_id) {
  string line;
  ifstream file;
  file.open("config.cfg");

  while (file && getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }

    int split_position = line.find(' ');
    int port = stoi(line.substr(split_position+1, line.length()));
    if (port != node_id) {
      Node x;
      x.port = port;
      x.ip = line.substr(0,split_position);
      v.push_back(x);
    }
  }
}

// Input validation to ensure that a valid node ID is entered for credit/debit operations
bool input_check(int node_id, vector<Node> &v) {
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
  cout << endl << endl;;
  cout << "1. Trigger Snapshot / Return Marker" << endl;
  cout << "2. Show Last Saved State" << endl;
  cout << "3. Credit Amount" << endl;
  cout << "4. Debit Amount" << endl;
  cout << endl << endl;;
  cout << "Choice: ";
  cin >> choice;

  if (choice < 0 || choice > 4) {
    cout << "\n\nInvalid choice\n\n";
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
  int SERVER_PORT = stoi(argv[1]);
  int CLIENT_PORT = SERVER_PORT - 1;

  cout<<"\n\nNode ID = " << SERVER_PORT;

  // Initializing clients
  vector<Node> nodes;
  initialize_clients(nodes, SERVER_PORT);
  string num_of_nodes = to_string(nodes.size() + 1);


  try {

    // Creating and binding socket to port
    UDP s;
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
        cout << endl << "LAST SAVED STATE" << endl << endl;
        s.send("127.0.0.1", SERVER_PORT, "L");
        Datagram d1 = s.receive();
        cout << endl << d1.data << endl << endl;
        Datagram d2 = s.receive();
        if (d2.data.length() > 0) {
          cout << "Staged Transations" << endl;
          cout << d2.data << endl;
        }
      }

      // Credit / debit operations
      else if (choice == 3 || choice == 4) {
        cout << endl << endl;
        cout << "Enter node ID to credit/debit: ";
        cin >> node_id;
        cout << endl << "Enter Amount: $";
        cin >> amount;
        cout << endl << endl;

        if (!input_check(node_id,nodes)) {
          cout << "Entered Node ID not in the config file" << endl << endl;
          continue;
        }

        string out_msg, in_msg;
        if (choice == 3) {
          out_msg = "C " + to_string(amount);
          in_msg = "D " + to_string(amount);
        }
        else if (choice == 4) {
          out_msg = "D " + to_string(amount);
          in_msg = "C " + to_string(amount);
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

  catch (Exception &e) {
    cout << e.exp() << endl << endl;
  }
  return 1;
}
