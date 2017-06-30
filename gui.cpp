// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for Gui class
//

#include "gui.h"

#include <iostream>
#include <memory>
#include <vector>

#include "packages.h"
#include "packages_legacy.h"
#include "util.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::move;
using std::stoul;
using std::string;
using std::vector;
using std::unique_ptr;

namespace {
void Help() {
  cout << "find [-f] [count=50] [string]: Find packages containing [string]." << '\n';
  cout << "\tPrompt user if there are more than [count] results." << '\n';
  cout << "\t[-f]: Only show results beginning with [string]." << '\n';
  cout << "view [--raw] [package]: View the data of [package]" << '\n';
  cout << "\t[--raw]: Show the raw version as opposed to deparsed version." << '\n';
  cout << "sort [count=1024] [filename=out.txt]: Sort and output the file to out.txt" << '\n';
  cout << "\tShow progress every [count] headers dumped." << '\n';
  cout << "\tSorted file will be dumped to [filename]." << '\n';
  cout << "compare [filename]: Compares the headers of the currently loaded file with [filename]" << '\n';
  cout << '\n';
  cout << "exit: Exit the application" << '\n';

  cout.flush();
}


}  // namespace

Gui::Gui(Packages* package) : packages_(move(package)), package_ver_(PackageVer::kCurrent) {}

Gui::Gui(PackagesLegacy* package) : package_legacy_(move(package)), package_ver_(PackageVer::kLegacy) {}

void Gui::MainMenu() {
  while (true) {
    system("cls");
    cout << "Warframe Packages Deparser: Loaded " << GetFileName() << '\n';
    cout << "Found: " << GetSize() << " records" << '\n';
    cout << '\n';
    cout << "Type 'help' to see all available commands." << '\n';
    cout << '\n';
    cout << "> ";
    cout.flush();

    string response{""};
    getline(cin, response);

    auto argv = SplitString(response, " ");
    if (argv.size() == 0) { continue; }

    if (ParseCommand(argv, true)) {
      return;
    }

    cout << "\nPress [ENTER] to continue..." << endl;
    getline(cin, response);
  }
}

bool Gui::ParseCommand(const vector<string>& args, bool is_interactive) {
  string input = args.at(0);
  auto argv = vector<string>(args.begin() + 1, args.end());

  if (input == "exit" && is_interactive) {
    return true;
  } else if (input == "help") {
    Help();
  } else if (input == "find") {
    Find(args);
  } else if (input == "view") {
    View(args);
  } else if (input == "sort") {
    Sort(args);
  } else if (input == "compare") {
    Compare(args);
  } else {
    cout << input << ": Not a valid command" << endl;
  }

  return false;
}

auto Gui::GetFileName() const -> string {
  switch (package_ver_) {
    case PackageVer::kLegacy:
      return package_legacy_->GetFilename();
    case PackageVer::kCurrent:
      return packages_->GetFilename();
    default:
      // all cases covered
      return "";
  }
}

auto Gui::GetSize() const -> size_t {
  switch (package_ver_) {
    case PackageVer::kLegacy:
      return package_legacy_->GetSize();
    case PackageVer::kCurrent:
      return packages_->GetSize();
    default:
      // all cases covered
      return 0;
  }
}

void Gui::Find(const vector<string>& args) const {
  enum class SearchMode {
    kDefault,
    kFront
  };

  // initialize all parameters
  string find_s{""};
  unsigned int max_count{50};
  SearchMode mode{SearchMode::kDefault};

  for (auto&& arg : args) {
    if (arg.substr(0, 6) == "count=") {
      try {
        max_count = stoul(arg.substr(6));
      } catch (std::invalid_argument ex_ia) {
        cerr << "Argument provided to [count] is not a number" << endl;
        return;
      }
    } else if (arg == "-f") {
      mode = SearchMode::kFront;
    } else {
      find_s = arg;
    }
  }

  if (find_s == "") {
    cout << "No search string provided." << endl;
    return;
  }

  switch (package_ver_) {
    case PackageVer::kLegacy:
      switch (mode) {
        case SearchMode::kFront:
          package_legacy_->SuggestSimilar(find_s, max_count);
          break;
        default:
          cout << "This mode is currently not supported with legacy packages." << endl;
          break;
      }
      break;
    case PackageVer::kCurrent:
      switch (mode) {
        case SearchMode::kDefault:
          packages_->Find(find_s, false, max_count);
          break;
        case SearchMode::kFront:
          packages_->Find(find_s, true, max_count);
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

void Gui::View(const vector<string>& args) const {
  enum class ViewMode {
    kDefault,
    kRaw
  };

  // initialize all parameters
  string package{""};
  ViewMode mode{ViewMode::kDefault};

  for (auto&& arg : args) {
    if (arg == "--raw") {
      mode = ViewMode::kRaw;
    } else {
      package = arg;
    }
  }

  if (package == "") {
    cout << "Please supply a package name." << endl;
    return;
  }

  switch (package_ver_) {
    case PackageVer::kLegacy:
      switch (mode) {
        case ViewMode::kDefault:
          package_legacy_->DeparseHeader(package);
          break;
        case ViewMode::kRaw:
          package_legacy_->OutputHeaderRaw(package);
          break;
        default:
          cout << "This mode is currently not supported with legacy packages." << endl;
          break;
      }
      break;
    case PackageVer::kCurrent:
      switch (mode) {
        case ViewMode::kDefault:
          packages_->OutputHeader(package);
          break;
        case ViewMode::kRaw:
          packages_->OutputHeaderRaw(package);
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

void Gui::Sort(const vector<string>& args) const {
  // initialize all parameters
  unsigned int count{1024};
  string filename{"out.txt"};

  for (auto&& arg : args) {
    if (arg.substr(0, 6) == "count=") {
      try {
        count = stoul(arg.substr(6));
      } catch (std::invalid_argument ex_ia) {
        cerr << "Argument provided to [count] is not a number" << endl;
        return;
      }
    } else if (arg.substr(0, 9) == "filename=") {
      filename = arg.substr(9);
    } else {
      filename = arg;
    }
  }

  if (filename == "") {
    cout << "Output filename cannot be empty" << endl;
    return;
  }

  switch (package_ver_) {
    case PackageVer::kLegacy:
      cout << "This mode is currently not supported with legacy packages." << endl;
      break;
    case PackageVer::kCurrent:
      packages_->SortFile(filename, count);
      break;
    default:
      // all cases covered
      break;
  }
}

void Gui::Compare(const vector<string>& args) const {
  string filename{""};

  for (auto&& arg : args) {
    filename = arg;
  }

  if (filename == "") {
    cout << "Please supply a filename." << endl;
    return;
  }

  switch (package_ver_) {
    case PackageVer::kLegacy:
      cout << "This mode is currently not supported with legacy packages." << endl;
      break;
    case PackageVer::kCurrent:
      packages_->Compare(filename);
      break;
    default:
      // all cases covered
      break;
  }
}
