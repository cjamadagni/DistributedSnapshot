/*
Server code for a particular node.
@author Chirag Jamadagni
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
//#include "Socket.hpp"
#include "socket-wrapper.h"

using namespace std;

// Constant control information
#define INITIAL_BANK_BALANCE 100
string file_suffix;
string file_prefix = "Data/";

// Utility function to determine whether logged entry is credit/debit/invalid
int recover_utility(string input) {
  int pos = input.find(' ') + 1;
  char operation = input.at(pos);

  if (operation == 'C') {
    return stoi(input.substr(pos + 2));
  }
  else if (operation == 'D') {
    return -1 * stoi(input.substr(pos + 2));
  }
  else {
    cout << endl << endl << "Recovery Log is Corrupted.\n\nTerminate Simulation and run setup.sh script." << endl << endl;
    exit(0);
    return -1;
  }
}

// Function to recover state of node on failure
int recover() {
  string balance;
  ifstream ledger;
  string file_name = file_prefix + "ledger" + file_suffix + ".txt";
  ledger.open(file_name);
  ledger >> balance;
  ledger.close();

  if (balance == "N") {
    return INITIAL_BANK_BALANCE;
  }
  else {

    int b = stoi(balance);

    string line;
    ifstream logs;
    string file_name = file_prefix + "log" + file_suffix + ".txt";
    logs.open(file_name);

    while (logs && getline(logs, line)) {
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
  ofstream file;
  string file_name = file_prefix + "ledger" + file_suffix + ".txt";
  file.open(file_name, ios::out);
  file << balance;
  file.close();

  // Deleting old log
  string logfile_name = file_prefix + "log" + file_suffix + ".txt";
  ofstream log_file;
  log_file.open(logfile_name, ofstream::out | ofstream::trunc);
  log_file.close();

  return 1;
}

// Function to capture channel state/ staged transactions
void stage_transaction(int node_id, string input) {
  string t = to_string(node_id+1) + " " + input;
  //string t = input + " " + to_string(node_id+1);
  ofstream file;
  string file_name = file_prefix + "log" + file_suffix + ".txt";
  file.open(file_name, ios::app);
  file << t;
  file << "\n";
  file.close();
}

// Function to obtain the last checkpointed ledger value
string last_checkpointed_node_state(int *flag) {
  string balance;
  ifstream file;
  string file_name = file_prefix + "ledger" + file_suffix + ".txt";
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
string last_checkpointed_channel_states() {
  string staged_transactions = "";
  string line;
  ifstream file;
  string file_name = file_prefix + "log" + file_suffix + ".txt";
  file.open(file_name);

  while (file && getline(file, line)) {
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
int credit(string input, int *balance) {
  int credit_amount = stoi(input.substr(2));
  *balance += credit_amount;
  return 1;
}

// Function to debit an amount from the bank balance
int debit(string input, int *balance) {
  int debit_amount = stoi(input.substr(2));
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
  int SERVER_PORT = stoi(argv[1]);
  int CLIENT_PORT = SERVER_PORT - 1;
  file_suffix = argv[1];

  // Recovering from failure
  bank_balance = recover();
  cout << endl << "Current Balance = " << bank_balance << endl << endl;

  try {
    UDP s;
    s.bind(SERVER_PORT);
    Datagram control_info = s.receive();
    num_of_nodes = stoi(control_info.data);

    while (true) {
      Datagram d = s.receive();

      // Handling snapshot/marker messages
      if (d.data == "S") {
        cout << endl << "Received checkpoint marker." << endl << endl;
        marker_count++;

        if (marker_count == 1) {
          return_code = checkpoint(bank_balance);
          checkpoint_active = true;
        }
        if (marker_count >= num_of_nodes) {
          cout << endl << "Snapshot complete. Received all markers. Saved all node and channel states." << endl << endl;
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
          cout << "Current Balance = " << bank_balance << endl << endl;
        }
        else {
          return_code = credit(d.data, &bank_balance);
          cout << "Current Balance = " << bank_balance << endl << endl;
        }
      }

      // handling debit messages
      else if (d.data.at(0) == 'D') {
        if (checkpoint_active) {
          return_code = debit(d.data, &bank_balance);
          stage_transaction(d.address.port, d.data);
          //marker_count++;
          cout << "Current Balance = " << bank_balance << endl << endl;
        }
        else {
          return_code = debit(d.data, &bank_balance);
          cout << "Current Balance = " << bank_balance << endl << endl;
        }
      }

    }
  }
  catch(Exception &e) {
    cout << e.exp() << endl << endl;
  }
  return 1;
}
