#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "gui.h"
#include "packages.h"
#include "packages_legacy.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::make_unique;
using std::move;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {
const std::string kBuildString = "0.6.0-beta";

Gui::PackageVer ReadArgs(const vector<string>&, string& filename);
void OutputVersionInfo();
void OutputHelp(const string& s);

void OutputHelp(const string& s) {
  string message{""};
  message += "Usage: " + s + " [OPTION]...\n";
  message += "  -f, --file=[FILE]\tread Packages.txt from [FILE]\n";
  message += "      --legacy\t\tread file with legacy format\n";
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

Gui::PackageVer ReadArgs(const vector<string>& args, string& filename) {
  string file = "../Packages.txt";
  Gui::PackageVer package_ver = Gui::PackageVer::kCurrent;

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
    } else if (*it == "--legacy") {
      package_ver = Gui::PackageVer::kLegacy;
    } else if (it->substr(0, 2) == "--") {
      cout << "Warning: Unrecognized option " << *it << endl;
    }
  }

  if (file.at(0) != '.' && file.at(1) != '.') {
    file.insert(0, "./");
  }

  unique_ptr<ifstream> file_stream = make_unique<ifstream>(file);
  if (!file_stream->good()) {
    cout << file << ": File not found. Exiting." << endl;
    exit(0);
  }

  filename = file;
  return package_ver;
}
}  // namespace

int main(int argc, char* argv[]) {
  // read input arguments
  vector<string> argvec(argv, argv + argc);

  string filename{};
  Gui::PackageVer ver = ReadArgs(argvec, filename);
  unique_ptr<ifstream> file_stream = make_unique<ifstream>(filename);
  unique_ptr<Packages> package = nullptr;
  unique_ptr<PackagesLegacy> package_legacy = nullptr;

  if (!file_stream->good()) {
    cout << filename << ": File not found. Exiting." << endl;
    exit(0);
  }

  switch (ver) {
    case Gui::PackageVer::kCurrent:
      package = make_unique<Packages>(filename, move(file_stream));
      break;
    case Gui::PackageVer::kLegacy:
      package_legacy = make_unique<PackagesLegacy>(filename, move(file_stream));
      break;
    default:
      // all cases covered
      break;
  }

  unique_ptr<Gui> g = nullptr;
  if (package != nullptr) {
    g = make_unique<Gui>(package.get());
  } else if (package_legacy != nullptr) {
    g = make_unique<Gui>(package_legacy.get());
  } else {
    cout << "Error when initializing packages!" << endl;
    exit(0);
  }

  g->MainMenu();

  return 0;
}