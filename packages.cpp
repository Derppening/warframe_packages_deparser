//
// Created by david on 3/5/2017.
//

#include "packages.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::make_unique;
using std::map;
using std::ofstream;
using std::string;
using std::unique_ptr;
using std::vector;

Packages::Packages(string n, unique_ptr<ifstream> ifs)
    : ifs_(std::move(ifs)), filename_(n), headers_(make_unique<map<string, unsigned int>>()) {
  ParseFile(ifs_.get());
}

void Packages::OutputHeaderRaw(string header) {
  auto search{headers_->find(header)};
  if (search == headers_->end()) {
    cout << header << ": Header not found." << endl;
    return;
  }
  system("cls");

  cout << "Package Name: " << header << endl;
  cout << "Line Number in File: " << search->second + 1 << endl;

  auto contents{GetHeaderContents(header)};
  for (auto&& l : *contents) {
    cout << l << endl;
  }
}

void Packages::OutputHeader(string header) {
  auto search = headers_->find(header);
  if (search == headers_->end()) {
    cout << header << ": Header not found." << endl;
    return;
  }
  system("cls");

  cout << "Package Name: " << header << endl;
  cout << "Line Number in File: " << search->second + 1 << endl;

  auto contents{GetHeaderContents(header)};
  for (auto&& l : *contents) {
    cout << l << endl;
  }
}

void Packages::FindFront(std::string header, unsigned int max_size) {
  std::transform(header.begin(), header.end(), header.begin(), ::tolower);
  unique_ptr<vector<string>> matches{make_unique<vector<string>>()};
  for (auto&& p : *headers_) {
    string s{p.first};
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s.substr(0, header.size()) == header) {
      matches->emplace_back(p.first);
    }
  }
  if (matches->size() > max_size) {
    cout << "Display all " << matches->size() << " possibilities? (y/n) ";
    string response{};
    getline(cin, response);
    if (response != "y") {
      return;
    }
  }
  for (auto&& e : *matches) {
    cout << e << '\n';
  }
  cout << endl;
  cout << matches->size() << " entries." << endl;
}

void Packages::Find(std::string header, unsigned int max_size) {
  std::transform(header.begin(), header.end(), header.begin(), ::tolower);
  unique_ptr<vector<string>> matches = make_unique<vector<string>>();
  for (auto&& p : *headers_) {
    string s{p.first};
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s.find(header) != string::npos) {
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
    cout << e << '\n';
  }
  cout << endl;
  cout << matches->size() << " entries." << endl;
}

void Packages::SortFile(string outfile, unsigned int notify_count) {
  unique_ptr<ifstream> instream = make_unique<ifstream>(filename_);
  unique_ptr<map<string, vector<string>>> contents = make_unique<map<string, vector<string>>>();
  unique_ptr<ofstream> outstream = make_unique<ofstream>(outfile);

  cout << "Reading file, please wait..." << endl;

  string category{};
  string buffer_line{};
  for (auto i = 0; getline(*instream, buffer_line); ++i) {
    if (buffer_line == "") {
      continue;
    }
    auto is_category = buffer_line.substr(0, 17) == "~FullPackageName=";
    if (is_category) {
      category = buffer_line.substr(17);
      contents->emplace(category, vector<string>());
      contents->at(category).emplace_back(buffer_line);
    } else if (category == "") {
      continue;
    } else {
      ConvertTabToSpace(buffer_line);
      contents->at(category).emplace_back(buffer_line);
    }
  }

  instream->close();

  auto count{0};
  const auto total{contents->size()};

  for (auto&& p : *contents) {
    if (++count % notify_count == 0) {
      cout << "On header: " << count << "/" << total << endl;
    }
    for (auto&& l : p.second) {
      *outstream << l << '\n';
    }
    outstream->flush();
  }

  outstream->close();
}

void Packages::ParseFile(ifstream* ifs) {
  cout << "Reading file, please wait..." << endl;
  string buffer_line{};
  for (auto i = 0; getline(*ifs, buffer_line); ++i) {
    if (buffer_line == "") {
      continue;
    }
    auto start_of_category = buffer_line.substr(0, 17) == "~FullPackageName=";
    if (start_of_category) {
      string category = buffer_line.substr(17);
      headers_->emplace(category, i);
    }
  }
}

unique_ptr<ifstream> Packages::GotoLine(unsigned int line) {
  auto file = make_unique<ifstream>(filename_);
  file->seekg(std::ios::beg);
  for (unsigned int it = 0; it < line - 1; ++it) {
    file->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  return file;
}

void Packages::ConvertTabToSpace(string& str) {
  string::size_type n = 0;
  while ((n = str.find('\t', n)) != string::npos) {
    str.replace(n, 1, "  ");
    n += 2;
  }
}

unique_ptr<vector<string>> Packages::GetHeaderContents(string header, bool inc_header) {
  unique_ptr<vector<string>> content = make_unique<vector<string>>();

  auto search{headers_->find(header)};
  if (search == headers_->end()) {
    return content;
  }

  auto index{search->second + 1 + static_cast<int>(!inc_header)};

  auto fs{GotoLine(index)};

  string line{};
  for (auto it{index}; getline(*fs, line); ++it) {
    if (it != index && line.substr(0, 17) == "~FullPackageName=") {
      break;
    }
    ConvertTabToSpace(line);
    content->push_back(line);
  }

  return content;
}
