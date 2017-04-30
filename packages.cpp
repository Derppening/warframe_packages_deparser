//
// Created by david on 30/4/2017.
//

#include "packages.h"

#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::make_pair;
using std::make_unique;
using std::map;
using std::string;
using std::unique_ptr;
using std::vector;

Packages::Packages(string n, unique_ptr<ifstream> ifs) {
  filename_ = n;

  // create the unique_ptrs
  header_contents_ = make_unique<map<string, vector<string>>>();

  ParseFile(ifs.get());
}

void Packages::OutputHeaderRaw(string s) {
  auto search = header_contents_->find(s);
  if (search != header_contents_->end()) {
    for (auto&& l : search->second) {
      cout << l << endl;
    }
  } else {
    cout << s << ": Header not found." << endl;
  }
}

void Packages::DeparseHeader(string header) {
  auto search = header_contents_->find(header);
  if (search == header_contents_->end()) {
    cout << header << ": Header not found." << endl;
    return;
  }
  system("cls");

  cout << "Package Name: " << header << endl;
  for (unsigned int i = 0; i < search->second.size(); ++i) {
    string line{static_cast<string&&>(search->second.at(i))};
    if (i == 0) {
      auto base_package_start = line.find('[');
      auto base_package_end = line.find(']');
      string base_package = line.substr(base_package_start + 1, base_package_end - base_package_start - 1);
      if (base_package == "") {
        base_package = "None";
      }
      cout << "Base package: " << base_package << '\n';
      cout << "Hex: " << line.substr(line.size() - 18) << "\n\n";
    } else {
      if (line.find("\"\"") != string::npos) {
        line.replace(line.find("\"\""), 2, "(empty)");
      }
      auto equal_search = line.find('=');
      auto array_open_search = line.find('{');
      auto array_search = line.find("={");
      auto array_close_search = line.find('}');
      auto array_empty_search = line.find("={}");
      if (array_empty_search != string::npos) {
        cout << line.substr(0, array_empty_search) << ": (none)\n";
      } else if (equal_search != string::npos && array_search == string::npos) {
        cout << line.substr(0, equal_search) << ": " << line.substr(equal_search + 1) << '\n';
      } else {
        cout << line << '\n';
      }
    }
  }
  cout << endl;
}

void Packages::SuggestSimilar(string header, unsigned int max_size) {
  unique_ptr<vector<string>> matches = make_unique<vector<string>>();
  for (auto&& p : *header_contents_) {
    if (p.first.substr(0, header.size()) == header) {
      matches->emplace_back(p.first);
    }
  }
  if (matches->size() > max_size) {
    cout << "Display all " << matches->size() << " possibilities? (y/n) ";
    string response;
    getline(cin, response);
    if (response != "y") {
      return;
    }
  }
  for (auto&& e : *matches) {
    cout << e << endl;
  }
}

void Packages::ParseFile(ifstream* fstream) {
  cout << "Reading file, please wait..." << endl;
  string buffer_line{};
  string category{};
  int indent_space = 0;
  while (getline(*fstream, buffer_line)) {
    if (buffer_line == "") {
      continue;
    }
    auto end_of_category = buffer_line.find(']');
    if (buffer_line.at(0) == '[' && end_of_category != string::npos) {
      category = buffer_line.substr(1, end_of_category - 1);
      header_contents_->emplace(category, vector<string>());
      header_contents_->at(category).emplace_back(buffer_line.substr(end_of_category + 2));
      indent_space = 0;
    } else if (category == "") {
      continue;
    } else {
      if (buffer_line.find('}') != string::npos && buffer_line.find("={") == string::npos) {
        --indent_space;
      }
      if (indent_space < 0) { indent_space = 0; }
      for (int i = 0; i < indent_space; ++i) {
        buffer_line = "  " + buffer_line;
      }
      header_contents_->at(category).emplace_back(buffer_line);
      if (buffer_line.at(static_cast<unsigned long long>(indent_space * 2)) == '{' ||
          (buffer_line.find("={") != string::npos && buffer_line.find('}') == string::npos)) {
        ++indent_space;
      }
    }
  }
}
