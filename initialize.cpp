#include <iostream>
#include <fstream>

int main() {
  std::string line;
  std::ifstream file;
  file.open("config.cfg");


  while (file && std::getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }
    
    std::string file1 = "ledger" + line + ".txt";
    std::string cmd2 = "touch log" + line + ".txt";
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
