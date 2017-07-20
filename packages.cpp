// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for Packages class
//

#include "packages.h"

#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using std::array;
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::make_pair;
using std::make_unique;
using std::map;
using std::ofstream;
using std::pair;
using std::sort;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {
vector<pair<string, string>> NormVarReplaceSet = {
    pair<string, string>("=", ": "),
    pair<string, string>("{}", "(empty hash)"),
    pair<string, string>("[]", "(empty array)"),
    pair<string, string>("SkipBuildTimePrice", "Rush Price (Platinum)"),
    pair<string, string>("PrimeSellingPrice", "Selling Price (Ducats)"),
    pair<string, string>("TradeCapability", "Can be Traded?"),
    pair<string, string>("SellingPrice", "Selling Price (Credits)"),
    pair<string, string>("PremiumPrice", "Price (Platinum)"),
    pair<string, string>("RegularPrice", "Price (Credits)"),
    pair<string, string>("BuildPrice", "Build Price (Platinum)"),
    pair<string, string>("BuildTime", "Build Time (Seconds)"),
    pair<string, string>("RO_ALWAYS", "Always"),
    pair<string, string>("RO_NEVER", "Never")
};

vector<pair<string, string>> BoolVarReplaceSet = {
    pair<string, string>("ShowInMarket", "Visible in Market?"),
    pair<string, string>("Tradeable", "Is Tradeable?"),
    pair<string, string>("Giftable", "Is Giftable?"),
    pair<string, string>("ExcludeFromCodex", "Hide in Codex?"),
    pair<string, string>("AvailableOnPvp", "Available in Conclave?"),
    pair<string, string>("AlwaysAvailable", "Always Available?"),
    pair<string, string>("CodexSecret", "Secret Entry in Codex?")
};

const array<pair<string, string>, 2> kBoolReplaceSet = {
    pair<string, string>("0", "false"),
    pair<string, string>("1", "true")
};

void PrettifyLine(std::string& s) {
  // do replacement for normal variables
  for (const auto& p_replace : NormVarReplaceSet) {
    auto cmp = s.find(p_replace.first);
    if (cmp != string::npos) {
      s.replace(cmp, p_replace.first.length(), p_replace.second);

      // quote absolute paths
      if (s.find("/Lotus") != string::npos) {
        s.replace(s.find("/Lotus"), 1, "\"/");
        s.push_back('\"');
      }
    }
  }

  // do replacement for boolean variables
  for (const auto& p_replace : BoolVarReplaceSet) {
    auto cmp = s.find(p_replace.first);
    if (cmp != string::npos) {
      s.replace(cmp, p_replace.first.length(), p_replace.second);

      // also replace the boolean values
      for (const auto& p_bool : kBoolReplaceSet) {
        auto val = s.find(p_bool.first);
        if (val != string::npos) {
          s.replace(val, p_bool.first.size(), p_bool.second);
          break;
        }
      }
    }
  }
}
}  // namespace

Packages::Packages(string n, unique_ptr<ifstream> ifs)
    : ifs_(std::move(ifs)), filename_(n), headers_(make_unique<map<string, unsigned int>>()) {
  if (!ifs_->good()) {
    throw std::runtime_error("Cannot open file");
  }

  ParseFile(ifs_.get());

  // sort the vectors!
  sort(NormVarReplaceSet.begin(), NormVarReplaceSet.end(), [](pair<string, string> a, pair<string, string> b) {
    return b.first.length() < a.first.length();
  });
  sort(BoolVarReplaceSet.begin(), BoolVarReplaceSet.end(), [](pair<string, string> a, pair<string, string> b) {
    return b.first.length() < a.first.length();
  });
}

void Packages::OutputHeader(string header, bool is_raw) {
  // find the header. return if we can't find it
  auto search = headers_->find(header);
  if (search == headers_->end()) {
    cout << header << ": Header not found." << endl;
    return;
  }

  system("cls");

  // read from file first. we need the base package
  cout << "Loading entry..." << endl;
  auto contents = GetHeaderContents(header);
  system("cls");

  if (is_raw) {
    // provide some raw information
    cout << "Package Name: " << header << endl;
    cout << "Line Number in File: " << search->second + 1 << endl;
    cout << endl;

    // display the contents
    for (auto&& l : *contents) {
      cout << l << endl;
    }
  } else {
    // provide some raw information
    cout << "Package Name: " << header << endl;
    if (contents->at(0).substr(0, 12) == "BasePackage=") {
      cout << "Base Package: " << contents->at(0).substr(12) << endl;
      contents->erase(contents->begin());
    }
    cout << endl;
    cout << "Line Number in File: " << search->second + 1 << endl;
    cout << endl;

    // display the contents
    for (auto& it : *contents) {
      PrettifyLine(it);

      cout << it << endl;
    }
  }
}

void Packages::Find(std::string header, bool search_front, unsigned int max_size) {
  std::transform(header.begin(), header.end(), header.begin(), ::tolower);
  unique_ptr<vector<string>> matches = make_unique<vector<string>>();

  // find all matches
  for (auto&& p : *headers_) {
    string s{p.first};
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (search_front && s.substr(0, header.size()) == header) {
      matches->emplace_back(p.first);
    } else if (!search_front && s.find(header) != string::npos) {
      matches->emplace_back(p.first);
    }
  }

  // check if we have more matches than max_size
  if (matches->size() > max_size) {
    cout << "Display all " << matches->size() << " possibilities? (y/n) ";
    string response;
    getline(cin, response);
    if (response != "y") {
      return;
    }
  }

  // display all matches and total count
  for (auto&& e : *matches) {
    cout << e << '\n';
  }
  cout << endl;
  cout << matches->size() << " entries." << endl;
}

void Packages::Compare(std::string cmp_filename) {
  unique_ptr<ifstream> cmp_filestream = make_unique<ifstream>(cmp_filename);
  if (!cmp_filestream->good()) {
    cout << cmp_filename << ": File not found." << endl;
    return;
  }
  auto cmp_file = make_unique<Packages>(cmp_filename, std::move(cmp_filestream));

  unique_ptr<vector<string>> has_current = make_unique<vector<string>>();
  unique_ptr<vector<string>> has_compare = make_unique<vector<string>>();

  const auto cmp_file_headers = cmp_file->GetHeaderPtr();

  cout << "Searching for new keys in current file..." << endl;
  for (auto&& h : *headers_) {
    if (cmp_file_headers->find(h.first) == cmp_file_headers->cend()) {
      has_current->emplace_back(h.first);
    }
  }

  cout << "Searching for new keys in comparing file..." << endl;
  for (auto&& h : *cmp_file_headers) {
    if (headers_->find(h.first) == headers_->cend()) {
      has_compare->emplace_back(h.first);
    }
  }

  system("cls");

  if (has_current->size() != 0) {
    cout << "Headers which only exist in current version: " << endl;
    for (auto&& h : *has_current) {
      cout << h << '\n';
    }
    cout << endl;
  }

  if (has_compare->size() != 0) {
    cout << "Headers which only exist in comparing version: " << endl;
    for (auto&& h : *has_compare) {
      cout << h << '\n';
    }
    cout << endl;
  }

  if (has_current->size() != 0 || has_compare->size() != 0) {
    cout << has_compare->size() << " additions, " << has_current->size() << " deletions" << endl;
  } else {
    cout << "Headers are identical." << endl;
  }
}

void Packages::SortFile(string outfile, unsigned int notify_count) {
  // initialize variables
  auto instream = make_unique<ifstream>(filename_);
  auto contents = make_unique<map<string, vector<string>>>();
  auto outstream = make_unique<ofstream>(outfile);

  cout << "Loading file, please wait..." << endl;

  // re-read the whole file into RAM
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

  unsigned count{0};
  const auto total = contents->size();

  // dump map into new file
  for (auto&& p : *contents) {
    if (++count % notify_count == 0) {
      cout << "Dumping header: " << count << "/" << total << endl;
    }
    for (auto&& l : p.second) {
      *outstream << l << '\n';
    }
    outstream->flush();
  }

  outstream->close();
}

void Packages::ReverseLookup(unsigned int line, bool is_interactive) {
  auto rev_headers = make_unique<map<unsigned int, string>>();

  cout << "Loading..." << endl;

  for (auto&& a : *headers_) {
    rev_headers->emplace(a.second, a.first);
  }

  auto i = rev_headers->end();
  for (auto it = rev_headers->begin(); it != rev_headers->end(); ++it) {
    if (it->first + 1 > line) {
      i = --it;
      break;
    }
  }

  system("cls");

  if (i != rev_headers->end()) {
    cout << "Entry at line " << line << ": " << i->second << endl;
    cout << "Entry begins at line " << i->first + 1 << endl << endl;
    if (is_interactive) {
      cout << "View Package Details? [y/N] ";
      string resp = "";
      getline(cin, resp);
      if (resp == "y" || resp == "Y") {
        OutputHeader(i->second, false);
      }
    }
  } else {
    cout << "No entry found at line " << line << endl << endl;
  }

}

void Packages::ParseFile(ifstream* ifs) {
  cout << "Reading file, please wait..." << endl;
  string buffer_line{};

  // read file and save with line numbers
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

auto Packages::GotoLine(unsigned int line) -> unique_ptr<ifstream> {
  auto file = make_unique<ifstream>(filename_);
  file->seekg(std::ios::beg);

  // skip line number of lines
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

auto Packages::GetHeaderContents(string header, bool inc_header) -> unique_ptr<vector<string>> {
  unique_ptr<vector<string>> content = make_unique<vector<string>>();

  auto search = headers_->find(header);
  if (search == headers_->end()) {
    return content;
  }

  auto index = search->second + 1 + static_cast<int>(!inc_header);

  auto fs = GotoLine(index);

  string line{};
  for (auto it = index; getline(*fs, line); ++it) {
    if (it != index && line.substr(0, 17) == "~FullPackageName=") {
      break;
    }
    ConvertTabToSpace(line);
    content->push_back(line);
  }

  return content;
}
