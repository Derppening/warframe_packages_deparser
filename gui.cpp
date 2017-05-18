//
// Created by david on 30/4/2017.
//

#include "gui.h"

#include <iostream>
#include <memory>
#include <vector>

#include "packages_legacy.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::move;
using std::stoul;
using std::string;
using std::vector;
using std::unique_ptr;

Gui::Gui(Packages& package) : packages_(&package), package_ver_(PackageVer::kCurrent) {}

Gui::Gui(PackagesLegacy& package) : package_legacy_(&package), package_ver_(PackageVer::kLegacy) {}

Gui::Gui(Packages* package) : packages_(move(package)), package_ver_(PackageVer::kCurrent) {}

Gui::Gui(PackagesLegacy* package) : package_legacy_(move(package)), package_ver_(PackageVer::kLegacy) {}

void Gui::MainMenu() {
  while (true) {
    system("cls");
    cout << "Warframe Packages Deparser: Loaded " << GetFileName() << '\n';
    cout << "Found: " << GetSize() << " records" << '\n';
    cout << '\n';
    cout << "find [-f] [max_count = 50] [string]: Find packages containing [string]." << '\n';
    cout << "\tNotify user if there are more than max_count results." << '\n';
    cout << "\t[-f]: Only show results beginning with [string]." << '\n';
    cout << "view [package]: View the deparsed data of [package]" << '\n';
    cout << "view raw [package]: View the raw data of [package]" << '\n';
    cout << '\n';
    cout << "exit: Exit the application" << '\n';
    cout << '\n';
    cout << "> ";

    string response{""};
    getline(cin, response);
    while (response.back() == ' ') {
      response.pop_back();
    }
    if (response == "exit") {
      break;
    } else if (response.substr(0, 8) == "find -f ") {
      unsigned int count = 50;
      response = response.substr(8);
      if (response.find(' ') != string::npos) {
        count = stoul(response.substr(0, response.find(' ')));
        response.erase(0, response.find(' ') + 1);
      }
      FindFront(response, count);
    } else if (response.substr(0, 5) == "find ") {
      unsigned int count = 50;
      response = response.substr(5);
      if (response.find(' ') != string::npos) {
        count = stoul(response.substr(0, response.find(' ')));
        response.erase(0, response.find(' ') + 1);
      }
      Find(response, count);
    } else if (response.substr(0, 9) == "view raw ") {
      if (response.size() >= 9) {
        OutputHeaderRaw(response.substr(9));
      } else {
        cout << "Please supply a package name." << endl;
      }
    } else if (response.substr(0, 5) == "view ") {
      if (response.size() >= 5) {
        OutputHeader(response.substr(5));
      } else {
        cout << "Please supply a package name." << endl;
      }
    } else {
      cout << response << ": Not a valid command" << endl;
    }
    cout << "\nPress [ENTER] to continue..." << endl;
    cin.ignore();
  }
}

string Gui::GetFileName() const {
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

size_t Gui::GetSize() const {
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

void Gui::OutputHeader(const string header) const {
  switch (package_ver_) {
    case PackageVer::kLegacy:
      package_legacy_->DeparseHeader(header);
      break;
    case PackageVer::kCurrent:
      packages_->OutputHeader(header);
      break;
    default:
      // all cases covered
      break;
  }
}

void Gui::OutputHeaderRaw(const string header) const {
  switch (package_ver_) {
    case PackageVer::kLegacy:
      package_legacy_->OutputHeaderRaw(header);
      break;
    case PackageVer::kCurrent:
      packages_->OutputHeaderRaw(header);
      break;
    default:
      // all cases covered
      break;
  }
}

void Gui::Find(const std::string substr, const unsigned int max_size) const {
  switch (package_ver_) {
    case PackageVer::kLegacy:
      cout << "Find is currently not supported with legacy packages." << endl;
      break;
    case PackageVer::kCurrent:
      packages_->Find(substr, max_size);
      break;
    default:
      // all cases covered
      break;
  }
}

void Gui::FindFront(const std::string header, const unsigned int max_size) const {
  switch (package_ver_) {
    case PackageVer::kLegacy:
      package_legacy_->SuggestSimilar(header, max_size);
      break;
    case PackageVer::kCurrent:
      packages_->FindFront(header, max_size);
      break;
    default:
      // all cases covered
      break;
  }
}
