// Copyright (c) 2017-2018 David Mak. All rights reserved.
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
#include "init.h"
#include "packages.h"
#include "log.h"
#include "util.h"

using std::cout;
using std::endl;

namespace {
/**
 * @brief List of program options.
 */
struct {
  Gui::PackageVer package_ver = Gui::PackageVer::kCurrent;

  std::string prettify_src = "";

  bool is_interactive = true;
  std::vector<std::string> ni_args;
} program_args;

void ReadArgs(const std::vector<std::string>& args, std::string* filename);

/**
 * @brief Parse all arguments in the command line.
 *
 * @param args Vector of arguments
 * @param filename Filename to read the file from
 */
void ReadArgs(const std::vector<std::string>& args, std::string* const filename) {
  std::string file = "./Packages.txt";
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
      cout << "Legacy format support has been discontinued." << endl;
      exit(0);
    } else if (*it == "--no-interactive" || *it == "-I") {
      program_args.is_interactive = false;
    } else if (*it == "--no-debug" || *it == "-D") {
      Log::Disable();
    } else if (*it == "-p") {
      program_args.prettify_src = *++it;
    } else if (it->substr(0, 11) == "--prettify=") {
      program_args.prettify_src = it->substr(11);
    } else if (!program_args.is_interactive && is_parse_ni_args) {
      program_args.ni_args.push_back(*it);
    } else if (*it == "--") {
      is_parse_ni_args = true;
    } else if (it->substr(0, 2) == "--" && it->length() != 2) {
      cout << "Warning: Unrecognized option " << *it << endl;
    }
  }

  if (file.at(0) != '.' && file.at(1) != '.') {
    file.insert(0, "./");
  }

  *filename = file;

  Log::SetFile("debug.log");
  Log::SetOverridePipe(Log::kFile);
  Log::UseOverridePipe(true);

  Log::d("Launching with arguments: ");
  for (std::size_t it = 0; it < args.size(); ++it) {
    Log::d("[" + std::to_string(it) + "] " + args[it]);
  }
  Log::d("Interpreting Package Version: " + std::to_string(static_cast<int>(program_args.package_ver)));
  Log::d(
      "Prettify Replacement Source: " + (program_args.prettify_src.empty() ? "(none)" : program_args.prettify_src));
  Log::d("Interactive Mode: " + std::string(program_args.is_interactive ? "true" : "false"));
  Log::d("Interactive Mode Arguments: " + JoinToString(program_args.ni_args, " "));
  Log::FlushFileBuf();
}
}  // namespace

auto main(int argc, char* argv[]) -> int {
  Init();

  // read input arguments
  std::vector<std::string> argvec(argv, argv + argc);

  std::string filename{};
  ReadArgs(argvec, &filename);

  auto file_stream = std::ifstream(filename);
  std::unique_ptr<Packages> package = nullptr;

  try {
    switch (program_args.package_ver) {
      case Gui::PackageVer::kCurrent:
        Log::v("Attempting to create Packages");
        package = std::make_unique<Packages>(filename, std::move(file_stream), std::move(program_args.prettify_src));
        break;
    }
  } catch (std::runtime_error& ex_runtime) {
    Log::e("Error while opening file: " + std::string(ex_runtime.what()));
    cout << "Error while opening file: " << ex_runtime.what() << endl;
    return 0;
  }
  Log::FlushFileBuf();

  // check if packages is properly initialized
  if (package == nullptr) {
    cout << "Error when initializing packages!" << endl;
    return 1;
  }

  Log::v("Invoking Gui::Gui(Packages)");
  Gui g(package.get());
  Log::FlushFileBuf();

  if (!program_args.is_interactive) {
    if (program_args.ni_args.empty()) {
      Log::w("No interactive mode arguments! Quitting");
      cout << "No arguments provided for non-interactive mode. Exiting." << endl;

      return 0;
    }

    Cui c(Cui::HintLevel::kNone);
    c.AddItem("Find", "find", std::bind(&Gui::Find, g, std::placeholders::_1, false));
    c.AddItem("View", "view", std::bind(&Gui::View, g, std::placeholders::_1));
    c.AddItem("Sort", "sort", std::bind(&Gui::Sort, g, std::placeholders::_1));
    c.AddItem("Compare", "compare", std::bind(&Gui::Compare, g, std::placeholders::_1));
    c.AddItem("json-struct", "json-struct", std::bind(&Gui::JsonStructure, g, std::placeholders::_1));
    c.AddItem("json-dump", "json-dump", std::bind(&Gui::JsonDump, g, std::placeholders::_1));
    c.AddItem("Help", "help", std::bind(&Gui::Help, g, false));

    Log::d("Invoking Cui::Parse()");
    c.Parse(JoinToString(program_args.ni_args, " "));
  } else {
    Log::d("Invoking Gui::MainMenu()");
    g.MainMenu();
  }

  return 0;
}