#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "gui.h"
#include "packages.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::make_unique;
using std::move;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {
const std::string kBuildString = "0.1";

Packages ReadArgs(const vector<string>&);
void OutputVersionInfo();
void OutputHelp(const string& s);

void OutputHelp(const string& s) {
  string message{""};
  message += "Usage: " + s + " [OPTION]...\n";
  message += "  -f, --file=[FILE]\tread Packages.txt from [FILE]\n";
  message += "      --help\t\tdisplay this help and exit\n";
  message += "      --version\t\toutput version information and exit\n\n";

  cout << message << endl;
}

void OutputVersionInfo() {
  string message{""};
  message += "Warframe Package Deparser " + kBuildString + "\n";
  message += "Copyright (C) 2017 David Mak\n";
  message += "Licensed under MIT.";

  cout << message << endl;
}

Packages ReadArgs(const vector<string>& args) {
  string file = "../Packages.txt";

  for (auto it = args.begin() + 1; it != args.end(); ++it) {
    if (*it == "--help") {
      OutputHelp(args.at(0));
      exit(0);
    } else if (*it == "--version") {
      OutputVersionInfo();
      exit(0);
    } else if (*it == "-f") {
      file = *++it;
    } else if (it->substr(0, 7) == "--file=") {
      file = it->substr(7);
    }
  }

  if (file.at(0) != '.' && file.at(1) != '.') {
    file.insert(0, "./");
  }

  unique_ptr<ifstream> file_stream = make_unique<ifstream>(file);
  if (!file_stream->is_open()) {
    cout << file << ": File not found. Exiting." << endl;
  }

  Packages p(file, move(file_stream));
  return p;
}
}  // namespace

int main(int argc, char* argv[]) {
  // read input arguments
  vector<string> argvec(argv, argv + argc);

  Packages p = ReadArgs(argvec);
  Gui g(p);
  g.MainMenu();

  return 0;
}