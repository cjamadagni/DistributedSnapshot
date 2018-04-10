#include <iostream>
#include <fstream>
#include <string>
#include "Socket.hpp"

// #define CLIENT_PORT 3000
// #define SERVER_PORT 2000

// Application Specific Details
#define INITIAL_BANK_BALANCE 100

int checkpoint(int balance) {
  std::ofstream file;
  file.open("ledger.txt", ios::out);
  file << balance;
  file.close();

  return 1;
}

void stage_transaction(int node_id, std::string input) {
  std::string t = std::to_string(node_id) + " " + input;
  std::ofstream file;
  file.open("temp.txt", ios::app);
  file << t;
  file << "\n";
  file.close();
}

std::string last_checkpointed_node_state(int *flag) {
  //TODO: handle channel states
  std::string balance;
  std::ifstream file;
  file.open("ledger.txt");
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
  file.open("transaction.txt");

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
  int bank_balance = INITIAL_BANK_BALANCE;
  int return_code;
  int marker_count = 1;
  bool checkpoint_active = false;
  int staged_bank_balance;

  //configuration parameters
  int SERVER_PORT = std::stoi(argv[1]);
  int CLIENT_PORT = SERVER_PORT - 1;

  try {
    Socket::UDP s;
    s.bind(SERVER_PORT);

    while (true) {
      Socket::Datagram d = s.receive();

      if (marker_count > 1) {
        std::ofstream file;
        file.open("transaction.txt", ios::out);
      }

      if (d.data == "S") {
        //marker_count++;

        if (marker_count == 1) {
          return_code = checkpoint(bank_balance);
          checkpoint_active = true;
        }
        if (marker_count == 4) {
          //s.send("127.0.0.1", CLIENT_PORT, "Successful Checkpoint");
          std::cout << endl << "Collected Node and Channel States" << endl << endl;
          marker_count = 1;
          checkpoint_active = false;
        }
        //TODO: Don't send back Successful message until all channel states get blocked
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
          std::cout << marker_count << endl;
        }
        else {
          return_code = credit(d.data, &bank_balance);
          std::cout << "Remaining Balance = " << bank_balance << endl << endl;
        }
      }
      else if (d.data.at(0) == 'D') {
        if (checkpoint_active) {
          return_code = credit(d.data, &bank_balance);
          stage_transaction(d.address.port, d.data);
          marker_count++;
          std::cout << marker_count << endl;
        }
        else {
          return_code = credit(d.data, &bank_balance);
          std::cout << "Remaining Balance = " << bank_balance << endl << endl;
        }
      }
    }
  }
  catch(Socket::Exception &e) {
    std::cout << e.what() << endl << endl;
  }
  return 1;
}
