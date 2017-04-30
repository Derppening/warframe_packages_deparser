//
// Created by david on 30/4/2017.
//

#include "gui.h"

#include <iostream>
#include <memory>
#include <vector>

#include "packages.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::move;
using std::stoul;
using std::string;
using std::vector;
using std::unique_ptr;

Gui::Gui(Packages& package) {
  package_ = &package;
}

void Gui::MainMenu() {
  while (true) {
    system("cls");
    cout << "Warframe Packages Deparser: Loaded " << package_->GetFilename() << '\n';
    cout << "Found: " << package_->GetSize() << " records" << '\n';
    cout << '\n';
    cout << "find [max_count = 10] [string]: Find packages beginning with [string]." << '\n';
    cout << "\tNotify user if there are more than max_count results." << '\n';
    cout << "view [package]: View the deparsed data of [package]" << '\n';
    cout << "view raw [package]: View the raw data of [package]" << '\n';
    cout << '\n';
    cout << "exit: Exit the application" << '\n';
    cout << '\n';
    cout << "> ";

    string response;
    getline(cin, response);
    while (response.back() == ' ') {
      response.pop_back();
    }
    if (response == "exit") {
      break;
    } else if (response.substr(0, 5) == "find ") {
      unsigned int count = 10;
      response = response.substr(5);
      if (response.find(' ') != string::npos) {
        count = stoul(response.substr(0, response.find(' ')));
        response.erase(0, response.find(' ') + 1);
      }
      package_->SuggestSimilar(response, count);
    } else if (response.substr(0, 9) == "view raw ") {
      if (response.size() >= 9) {
        package_->OutputHeaderRaw(response.substr(9));
      } else {
        cout << "Please supply a package name." << endl;
      }
    } else if (response.substr(0, 5) == "view ") {
      if (response.size() >= 5) {
        package_->DeparseHeader(response.substr(5));
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
