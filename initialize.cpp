/*
Code to create empty logs and ledgers.
@author Chirag Jamadagni
*/

#include <iostream>
#include <fstream>

using namespace std;

int main() {
  string line;
  ifstream file;
  file.open("config.cfg");

  // Reading topology info from config file
  while (file && getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }
    int split_position = line.find(' ');
    string temp = line.substr(split_position+1, line.length());
    string file1 = "Data/ledger" + temp + ".txt";
    string cmd2 = "touch Data/log" + temp + ".txt";
    const char* cmd = cmd2.c_str();

    // Initializing Ledger File
    ofstream outfile;
    outfile.open(file1);
    outfile << "N";
    outfile.close();

    // Initializing log file
    system(cmd);
  }

  file.close();
  return 1;
}
