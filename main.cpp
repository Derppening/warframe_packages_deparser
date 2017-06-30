// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// This file parses the program argument, and inflates the Gui class.
//

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "gui.h"
#include "packages.h"
#include "packages_legacy.h"
#include "util.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::make_unique;
using std::move;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {
const string kBuildString = "0.8.0-rc.2";

struct {
  Gui::PackageVer package_ver = Gui::PackageVer::kCurrent;
  bool is_interactive = true;
  vector<string> ni_args;
} program_args;

void ReadArgs(const vector<string>&, string& filename);
void OutputVersionInfo();
void OutputHelp(const string& s);

void OutputHelp(const string& s) {
  string message{""};
  message += "Usage: " + s + " [OPTION]... -- [MODE] [MODE_ARGS]...\n";
  message += "  -f, --file=[FILE]\tread Packages.txt from [FILE]\n";
  message += "      --no-interactive\tdisable interactive mode\n";
  message += "      --legacy\t\tread file with legacy format\n";
  message += "      --help\t\tdisplay this help and exit\n";
  message += "      --version\t\toutput version information and exit\n\n";
  message += "MODE and MODE_ARGS will only be parsed if \'--no-interactive\' is provided.\n";
  message += "For help on using interactive mode, provide \'help\' to MODE.\n";

  cout << message << endl;
}

void OutputVersionInfo() {
  string message{""};
  message += "Warframe Package Deparser " + kBuildString + "\n";
  message += "Copyright (C) 2017 David Mak\n";
  message += "Licensed under MIT.";

  cout << message << endl;
}

void ReadArgs(const vector<string>& args, string& filename) {
  string file = "./Packages.txt";
  bool is_parse_ni_args = false;

  for (auto it = args.begin() + 1; it != args.end(); ++it) {
    if (program_args.is_interactive && is_parse_ni_args) {
      cout << "Warning: Ignoring all arguments provided after \"--\" token" << endl;
      break;
    } else if (*it == "--help") {
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
      program_args.package_ver = Gui::PackageVer::kLegacy;
    } else if (*it == "--no-interactive") {
      program_args.is_interactive = false;
    } else if (!program_args.is_interactive && is_parse_ni_args) {
      program_args.ni_args.push_back(*it);
    } else if (*it == "--") {
      is_parse_ni_args = true;
    } else if (it->substr(0, 2) == "--" &&
        it->length() != 2) {
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
}
}  // namespace

auto main(int argc, char* argv[]) -> int {
  // read input arguments
  vector<string> argvec(argv, argv + argc);

  string filename{};
  ReadArgs(argvec, filename);
  unique_ptr<ifstream> file_stream = make_unique<ifstream>(filename);
  unique_ptr<Packages> package = nullptr;
  unique_ptr<PackagesLegacy> package_legacy = nullptr;

  if (!file_stream->good()) {
    cout << filename << ": File not found. Exiting." << endl;
    exit(0);
  }

  switch (program_args.package_ver) {
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

  if (!program_args.is_interactive) {
    if (program_args.ni_args.size() == 0) {
      cout << "No arguments provided for non-interactive mode. Exiting." << endl;
    }

    g->ParseCommand(program_args.ni_args, false);
  } else {
    g->MainMenu();
  }

  return 0;
}