// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for Gui class
//

#include "gui.h"

#include <iostream>
#include <limits>
#include <vector>

#include "cui.h"
#include "packages.h"
#include "log.h"
#include "util.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::stoul;
using std::string;
using std::vector;

Gui::Gui(Packages* package) : packages_(package)
{}

void Gui::MainMenu() {
  Cui c(Cui::HintLevel::kNone);
  c.AddItem("Find", "find", std::bind(&Gui::Find, this, std::placeholders::_1, true));
  c.AddItem("View", "view", std::bind(&Gui::View, this, std::placeholders::_1));
  c.AddItem("Sort", "sort", std::bind(&Gui::Sort, this, std::placeholders::_1));
  c.AddItem("Compare", "compare", std::bind(&Gui::Compare, this, std::placeholders::_1));
  c.AddItem("Help", "help", std::bind(&Gui::Help, this, true));
  c.AddDiv();
  c.AddItem("Exit", "exit", nullptr, true);

  bool b = false;
  while (!b) {
    system("cls");
    cout << "Warframe Packages Deparser: Loaded " << GetFileName() << '\n';
    cout << "Found: " << GetSize() << " records" << '\n';
    cout << '\n';
    cout << "Type 'help' to see all available commands." << '\n';
    cout << '\n';
    cout.flush();

    b = c.Inflate(true, true);
  }

  Log::i("Exiting interactive loop...");
}

auto Gui::GetFileName() const -> string {
  switch (package_ver_) {
    case PackageVer::kCurrent:
      return packages_->GetFilename();
    default:
      // all cases covered
      return "";
  }
}

auto Gui::GetSize() const -> size_t {
  switch (package_ver_) {
    case PackageVer::kCurrent:
      return packages_->GetSize();
    default:
      // all cases covered
      return 0;
  }
}

void Gui::Help(bool is_interactive) const {
  cout << "find [-f] [count=50] [string]: Find packages containing [string]." << '\n';
  cout << "\tPrompt user if there are more than [count] results." << '\n';
  cout << "\t[-f]: Only show results beginning with [string]." << '\n';
  cout << '\n';
  cout << "find line=[line]: Reverse lookup package name at [line]" << '\n';
  cout << '\n';
  cout << "view [--raw] [package]: View the data of [package]" << '\n';
  cout << "\t[--raw]: Show the raw version as opposed to prettify version." << '\n';
  cout << '\n';
  cout << "sort [count=1024] [filename=out.txt]: Sort and output the file to out.txt" << '\n';
  cout << "\tShow progress every [count] headers dumped." << '\n';
  cout << "\tSorted file will be dumped to [filename]." << '\n';
  cout << '\n';
  cout << "compare [filename]: Compares the headers of the currently loaded file with [filename]" << '\n';
  if (is_interactive) {
    cout << '\n';
    cout << "exit: Exit the application" << '\n';
  }

  cout.flush();
}

void Gui::Find(string args, bool is_interactive) const {
  enum class SearchMode {
    kDefault,
    kFront,
    kLine
  };

  vector<string> argv = SplitString(std::move(args), " ");

  // initialize all parameters
  string find_s;
  unsigned int max_count = 50;
  unsigned int line = 0;
  SearchMode mode = SearchMode::kDefault;

  for (auto&& arg : argv) {
    if (arg.substr(0, 6) == "count=") {
      try {
        max_count = static_cast<unsigned int>(stoul(arg.substr(6)));
      } catch (std::invalid_argument& ex_ia) {
        cerr << "Argument provided to [count] is not a number" << endl;
        return;
      }
    } else if (arg.substr(0, 5) == "line="){
      try {
        line = static_cast<unsigned int>(stoul(arg.substr(5)));
      } catch (std::invalid_argument& ex_ia) {
        cerr << "Argument provided to [line] is not a number" << endl;
        return;
      }
      mode = SearchMode::kLine;
    } else if (arg == "-f") {
      mode = SearchMode::kFront;
    } else {
      find_s = arg;
    }
  }

  if (find_s.empty() && mode != SearchMode::kLine) {
    cout << "No search string provided." << endl;
    return;
  }

  if (!is_interactive) {
    max_count = std::numeric_limits<unsigned>::max();
  }

  switch (package_ver_) {
    case PackageVer::kCurrent:
      switch (mode) {
        case SearchMode::kDefault:
          Log::i("Invoking Packages::Find(\"" + find_s + "\"...)");
          packages_->Find(find_s, false, max_count);
          break;
        case SearchMode::kFront:
          Log::i("Invoking Packages::Find(\"" + find_s + "\"...)");
          packages_->Find(find_s, true, max_count);
          break;
        case SearchMode::kLine:
          Log::i("Invoking Packages::ReverseLookup(" + std::to_string(line) + "...)");
          packages_->ReverseLookup(line, is_interactive);
          break;
        default:
          cout << "This mode is currently not supported with current packages." << endl;
          break;
      }
      break;
    default:
      // all cases covered
      break;
  }
}

void Gui::View(const std::string args) const {
  enum class ViewMode {
    kDefault,
    kRaw
  };

  vector<string> argv = SplitString(args, " ");

  // initialize all parameters
  string package;
  ViewMode mode = ViewMode::kDefault;

  for (auto&& arg : argv) {
    if (arg == "--raw") {
      mode = ViewMode::kRaw;
    } else {
      package = arg;
    }
  }

  if (package.empty()) {
    cout << "Please supply a package name." << endl;
    return;
  }

  switch (package_ver_) {
    case PackageVer::kCurrent:
      switch (mode) {
        case ViewMode::kDefault:
          Log::i("Invoking Packages::View(\"" + package + "\"...)");
          packages_->OutputHeader(package, false);
          break;
        case ViewMode::kRaw:
          Log::i("Invoking Packages::View(\"" + package + "\"...)");
          packages_->OutputHeader(package, true);
          break;
        default:
          cout << "This mode is currently not supported with current packages." << endl;
          break;
      }
      break;
    default:
      // all cases covered
      break;
  }
}

void Gui::Sort(const string args) const {
  vector<string> argv = SplitString(args, " ");

  // initialize all parameters
  unsigned int count{1024};
  string filename{"out.txt"};

  for (auto&& arg : argv) {
    if (arg.substr(0, 6) == "count=") {
      try {
        count = static_cast<unsigned int>(stoul(arg.substr(6)));
      } catch (std::invalid_argument& ex_ia) {
        cerr << "Argument provided to [count] is not a number" << endl;
        return;
      }
    } else if (arg.substr(0, 9) == "filename=") {
      filename = arg.substr(9);
    } else {
      filename = arg;
    }
  }

  if (filename.empty()) {
    cout << "Output filename cannot be empty" << endl;
    return;
  }

  switch (package_ver_) {
    case PackageVer::kCurrent:
      Log::i("Invoking Packages::SortFile()");
      packages_->SortFile(filename, count);
      break;
    default:
      // all cases covered
      break;
  }
}

void Gui::Compare(const string args) const {
  vector<string> argv = SplitString(args, " ");

  string filename;
  for (auto&& arg : argv) {
    filename = arg;
  }

  if (filename.empty()) {
    cout << "Please supply a filename." << endl;
    return;
  }

  switch (package_ver_) {
    case PackageVer::kCurrent:
      Log::i("Invoking Packages::Compare(\"" + filename + "\"...)");
      packages_->Compare(filename);
      break;
    default:
      // all cases covered
      break;
  }
}
