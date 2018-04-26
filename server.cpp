/*
Server code for a particular node.
@author Chirag Jamadagni
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "Socket.hpp"

// Constant control information
#define INITIAL_BANK_BALANCE 100
std::string file_suffix;
std::string file_prefix = "Data/";

// Utility function to determine whether logged entry is credit/debit/invalid
int recover_utility(std::string input) {
  int pos = input.find(' ') + 1;
  char operation = input.at(pos);

  if (operation == 'C') {
    return std::stoi(input.substr(pos + 2));
  }
  else if (operation == 'D') {
    return -1 * std::stoi(input.substr(pos + 2));
  }
  else {
    std::cout << endl << endl << "Recovery Log is Corrupted.\n\nTerminate Simulation and run setup.sh script." << endl << endl;
    std::exit(0);
    return -1;
  }
}

// Function to recover state of node on failure
int recover() {
  std::string balance;
  std::ifstream ledger;
  std::string file_name = file_prefix + "ledger" + file_suffix + ".txt";
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
    std::string file_name = file_prefix + "log" + file_suffix + ".txt";
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

// Function to checkpoint/ take snapshot
int checkpoint(int balance) {
  std::ofstream file;
  std::string file_name = file_prefix + "ledger" + file_suffix + ".txt";
  file.open(file_name, ios::out);
  file << balance;
  file.close();

  // Deleting old log
  std::string logfile_name = file_prefix + "log" + file_suffix + ".txt";
  std::ofstream log_file;
  log_file.open(logfile_name, std::ofstream::out | std::ofstream::trunc);
  log_file.close();

  return 1;
}

// Function to capture channel state/ staged transactions
void stage_transaction(int node_id, std::string input) {
  //std::string t = std::to_string(node_id+1) + " " + input;
  std::string t = input + " " + std::to_string(node_id+1);
  std::ofstream file;
  std::string file_name = file_prefix + "log" + file_suffix + ".txt";
  file.open(file_name, ios::app);
  file << t;
  file << "\n";
  file.close();
}

// Function to obtain the last checkpointed ledger value
std::string last_checkpointed_node_state(int *flag) {
  std::string balance;
  std::ifstream file;
  std::string file_name = file_prefix + "ledger" + file_suffix + ".txt";
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

// Function to obtain the staged transactions
std::string last_checkpointed_channel_states() {
  std::string staged_transactions = "";
  std::string line;
  std::ifstream file;
  std::string file_name = file_prefix + "log" + file_suffix + ".txt";
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

// Function to credit an amount to the bank balance
int credit(std::string input, int *balance) {
  int credit_amount = std::stoi(input.substr(2));
  *balance += credit_amount;
  return 1;
}

// Function to debit an amount from the bank balance
int debit(std::string input, int *balance) {
  int debit_amount = std::stoi(input.substr(2));
  *balance -= debit_amount;
  return 1;
}

int main(int argc, char** argv) {
  int bank_balance;
  int return_code;
  int marker_count = 0;
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

      // Handling snapshot/marker messages
      if (d.data == "S") {
        std::cout << endl << "Received checkpoint marker." << endl << endl;
        marker_count++;

        if (marker_count == 1) {
          return_code = checkpoint(bank_balance);
          checkpoint_active = true;
        }
        if (marker_count >= num_of_nodes) {
          std::cout << endl << "Snapshot complete. Received all markers. Saved all node and channel states." << endl << endl;
          marker_count = 0;
          checkpoint_active = false;
        }
      }

      // Handling return current state messages
      else if (d.data == "L") {
        int flag = 1;
        s.send("127.0.0.1", CLIENT_PORT, last_checkpointed_node_state(&flag));
        if (flag == 1) {
          s.send("127.0.0.1", CLIENT_PORT, last_checkpointed_channel_states());
        }
      }

      // Handling shutdown messages
      else if (d.data == "T") {
        s.close();
        break;
      }

      // Handling credit messages
      else if (d.data.at(0) == 'C') {

        if (checkpoint_active) {
          return_code = credit(d.data, &bank_balance);
          stage_transaction(d.address.port, d.data);
          //marker_count++;
          std::cout << "Current Balance = " << bank_balance << endl << endl;
        }
        else {
          return_code = credit(d.data, &bank_balance);
          std::cout << "Current Balance = " << bank_balance << endl << endl;
        }
      }

      // handling debit messages
      else if (d.data.at(0) == 'D') {
        if (checkpoint_active) {
          return_code = debit(d.data, &bank_balance);
          stage_transaction(d.address.port, d.data);
          //marker_count++;
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
