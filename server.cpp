#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Socket.hpp"

// Application Specific Details
#define INITIAL_BANK_BALANCE 100
std::string file_suffix;

int recover_utility(std::string input) {
  char operation = input.at(5);

  if (operation == 'C') {
    return std::stoi(input.substr(7));
  }
  else if (operation == 'D') {
    return -1 * std::stoi(input.substr(7));
  }
  else {
    std::cout << endl << endl << "Recovery Log is Corrupted.\n\nTerminate Simulation and run setup.sh script." << endl << endl;
    return -1;
  }
}

int recover() {
  std::string balance;
  std::ifstream ledger;
  std::string file_name = "ledger" + file_suffix + ".txt";
  ledger.open(file_name);
  ledger >> balance;
  ledger.close();

  if (balance == "N") {
    return INITIAL_BANK_BALANCE;
  }
  else {

    int b = std::stoi(balance);

    std::string line;
    std::ifstream logs;
    std::string file_name = "log" + file_suffix + ".txt";
    logs.open(file_name);

    while (logs && std::getline(logs, line)) {
      if (line.length() == 0) {
        continue;
      }

      b += recover_utility(line);
    }
    logs.close();

    return b;
  }


}

int checkpoint(int balance) {
  std::ofstream file;
  std::string file_name = "ledger" + file_suffix + ".txt";
  file.open(file_name, ios::out);
  file << balance;
  file.close();

  return 1;
}

void stage_transaction(int node_id, std::string input) {
  std::string t = std::to_string(node_id+1) + " " + input;
  std::ofstream file;
  std::string file_name = "log" + file_suffix + ".txt";
  file.open(file_name, ios::app);
  file << t;
  file << "\n";
  file.close();
}

std::string last_checkpointed_node_state(int *flag) {
  std::string balance;
  std::ifstream file;
  std::string file_name = "ledger" + file_suffix + ".txt";
  file.open(file_name);
  file >> balance;
  file.close();

  if (balance == "N" ) {
    return "No checkpoint taken yet.";
    *flag = 0;
  }
  else {
    return "Balance Amount = " + balance;
  }
}

std::string last_checkpointed_channel_states() {
  //TODO: handle channel states
  std::string staged_transactions = "";
  std::string line;
  std::ifstream file;
  std::string file_name = "log" + file_suffix + ".txt";
  file.open(file_name);

  while (file && std::getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }
    staged_transactions = staged_transactions + line;
    staged_transactions = staged_transactions + "\n";
  }

  file.close();
  return staged_transactions;
}

int credit(std::string input, int *balance) {
  int credit_amount = std::stoi(input.substr(2));
  *balance += credit_amount;
  return 1;
}

int debit(std::string input, int *balance) {
  int debit_amount = std::stoi(input.substr(2));
  *balance -= debit_amount;
  return 1;
}

int main(int argc, char** argv) {
  int bank_balance;
  int return_code;
  int marker_count = 1;
  bool checkpoint_active = false;
  int staged_bank_balance;
  int num_of_nodes;

  // Configuration parameters
  int SERVER_PORT = std::stoi(argv[1]);
  int CLIENT_PORT = SERVER_PORT - 1;
  file_suffix = argv[1];

  // Recovering from failure
  bank_balance = recover();
  std::cout << endl << "Current Balance = " << bank_balance << endl << endl;

  try {
    Socket::UDP s;
    s.bind(SERVER_PORT);
    Socket::Datagram control_info = s.receive();
    num_of_nodes = std::stoi(control_info.data);

    while (true) {
      Socket::Datagram d = s.receive();

      if (d.data == "S") {
        std::cout << endl << "Received checkpoint marker." << endl;

        if (marker_count == 1) {
          return_code = checkpoint(bank_balance);
          checkpoint_active = true;
        }
        if (marker_count >= num_of_nodes) {
          //s.send("127.0.0.1", CLIENT_PORT, "Successful Checkpoint");
          std::cout << endl << "Snapshot complete. Received all markers. Saved all node and channel states." << endl << endl;
          marker_count = 1;
          checkpoint_active = false;
        }
      }
      else if (d.data == "L") {
        int flag = 1;
        s.send("127.0.0.1", CLIENT_PORT, last_checkpointed_node_state(&flag));
        if (flag == 1) {
          s.send("127.0.0.1", CLIENT_PORT, last_checkpointed_channel_states());
        }
      }
      else if (d.data == "T") {
        s.close();
        break;
      }
      else if (d.data.at(0) == 'C') {

        if (checkpoint_active) {
          return_code = credit(d.data, &bank_balance);
          stage_transaction(d.address.port, d.data);
          marker_count++;
          std::cout << "Current Balance = " << bank_balance << endl << endl;
        }
        else {
          return_code = credit(d.data, &bank_balance);
          std::cout << "Current Balance = " << bank_balance << endl << endl;
        }
      }
      else if (d.data.at(0) == 'D') {
        if (checkpoint_active) {
          return_code = debit(d.data, &bank_balance);
          stage_transaction(d.address.port, d.data);
          marker_count++;
          std::cout << "Current Balance = " << bank_balance << endl << endl;
        }
        else {
          return_code = debit(d.data, &bank_balance);
          std::cout << "Current Balance = " << bank_balance << endl << endl;
        }
      }
    }
  }
  catch(Socket::Exception &e) {
    std::cout << e.what() << endl << endl;
  }
  return 1;
}
