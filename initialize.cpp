/*
Server code to create empty logs and ledgers.
@author Chirag Jamadagni
*/

#include <iostream>
#include <fstream>

int main() {
  std::string line;
  std::ifstream file;
  file.open("config.cfg");

  // Reading topology info from config file
  while (file && std::getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }
    int split_position = line.find(' ');
    std::string temp = line.substr(split_position+1, line.length());
    std::string file1 = "Data/ledger" + temp + ".txt";
    std::string cmd2 = "touch Data/log" + temp + ".txt";
    const char* cmd = cmd2.c_str();

    // Initializing Ledger File
    std::ofstream outfile;
    outfile.open(file1);
    outfile << "N";
    outfile.close();

    // Initializing log file
    std::system(cmd);
  }

  file.close();
  return 1;
}
