// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// This file parses the program argument, and inflates the Gui class.
//

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "cui.h"
#include "gui.h"
#include "packages.h"
#include "packages_legacy.h"
#include "log.h"
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
const string kBuildString = "0.9.0-beta.5";

struct {
  Gui::PackageVer package_ver = Gui::PackageVer::kCurrent;

  std::string prettify_src = "";

  bool is_interactive = true;
  vector<string> ni_args;
} program_args;

void ReadArgs(const vector<string>& args, string& filename);
void OutputVersionInfo();
void OutputHelp(const string& s);

void OutputHelp(const string& s) {
  string message;
  message += "Usage: " + s + " [OPTION]... -- [MODE] [MODE_ARGS]...\n";
  message += "  -d, --debug\t\tenable logging\n";
  message += "  -f, --file=[FILE]\tread Packages.txt from [FILE]\n";
  message += "      --legacy\t\tread file with legacy format\n";
  message += "  -I, --no-interactive\tdisable interactive mode\n";
  message += "  -p, --prettify=[FILE]\timport prettifying replacement pairs from [FILE]\n";
  message += "      --help\t\tdisplay this help and exit\n";
  message += "      --version\t\toutput version information and exit\n\n";
  message += "MODE and MODE_ARGS will only be parsed if \'--no-interactive\' is provided.\n";
  message += "For help on using interactive mode, provide \'help\' to MODE.\n";

  cout << message << endl;
}

void OutputVersionInfo() {
  string message;
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
    }

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
      program_args.package_ver = Gui::PackageVer::kLegacy;
    } else if (*it == "--no-interactive" || *it == "-I") {
      program_args.is_interactive = false;
    } else if (*it == "--debug" || *it == "-d") {
      Log::Enable();
    } else if (*it == "-p") {
      program_args.prettify_src = *++it;
    } else if (it->substr(0, 11) == "--prettify=") {
      program_args.prettify_src = it->substr(11);
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

  filename = file;

  Log::SetFile("debug.log");
  Log::SetOverridePipe(Log::kFile);
  Log::UseOverridePipe(true);

  Log::d("Launching with arguments: ");
  for (std::size_t it = 0; it < args.size(); ++it) {
    Log::d("[" + std::to_string(it) + "] " + args[it]);
  }
  Log::d("Interpreting Package Version: " + std::to_string(static_cast<int>(program_args.package_ver)));
  Log::d("Prettify Replacement Source: " + (program_args.prettify_src.empty() ? "(none)" : program_args.prettify_src));
  Log::d("Interactive Mode: " + std::string(program_args.is_interactive ? "true" : "false"));
  Log::d("Interactive Mode Arguments: " + JoinToString(program_args.ni_args, " "));
  Log::FlushFileBuf();
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

  try {
    switch (program_args.package_ver) {
      case Gui::PackageVer::kCurrent:
        Log::v("Attempting to create Packages");
        package = make_unique<Packages>(filename, move(file_stream), program_args.prettify_src);
        break;
      case Gui::PackageVer::kLegacy:
        Log::v("Attempting to create PackagesLegacy");
        package_legacy = make_unique<PackagesLegacy>(filename, move(file_stream));
        break;
      default:
        throw std::runtime_error("Bad State");
    }
  } catch (std::runtime_error& ex_runtime) {
    Log::e("Error while opening file: " + std::string(ex_runtime.what()));
    cout << "Error while opening file: " << ex_runtime.what() << endl;
    return 0;
  }
  Log::FlushFileBuf();

  unique_ptr<Gui> g = nullptr;
  if (package != nullptr) {
    Log::v("Invoking Gui::Gui(Packages)");
    g = make_unique<Gui>(package.get());
  } else if (package_legacy != nullptr) {
    Log::v("Invoking Gui::Gui(PackagesLegacy)");
    g = make_unique<Gui>(package_legacy.get());
  } else {
    cout << "Error when initializing packages!" << endl;
    return 1;
  }
  Log::FlushFileBuf();

  if (!program_args.is_interactive) {
    if (program_args.ni_args.empty()) {
      Log::w("No interactive mode arguments! Quitting");
      cout << "No arguments provided for non-interactive mode. Exiting." << endl;
    }

    Cui c(Cui::HintLevel::kNone);
    c.AddItem("Find", "find", std::bind(&Gui::Find, *g, std::placeholders::_1, false));
    c.AddItem("View", "view", std::bind(&Gui::View, *g, std::placeholders::_1));
    c.AddItem("Sort", "sort", std::bind(&Gui::Sort, *g, std::placeholders::_1));
    c.AddItem("Compare", "compare", std::bind(&Gui::Compare, *g, std::placeholders::_1));
    c.AddItem("Help", "help", std::bind(&Gui::Help, *g, false));

    Log::d("Invoking Cui::Parse()");
    c.Parse(JoinToString(program_args.ni_args, " "));
  } else {
    Log::d("Invoking Gui::MainMenu()");
    g->MainMenu();
  }

  return 0;
}