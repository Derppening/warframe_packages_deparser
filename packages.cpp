// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for Packages class
//

#include "packages.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "config_file.h"
#include "log.h"
#include "timer.h"

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

namespace {
struct StrCompare {
  bool operator()(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) {
      return b.length() < a.length();
    }
    return a < b;
  }
};

map<string, string, StrCompare> NormVarReplaceSet;
map<string, string, StrCompare> BoolVarReplaceSet;
map<string, string, StrCompare> LotusVarReplaceSet;
map<string, string, StrCompare> kSyntaxReplaceSet;
map<string, string, StrCompare> kBoolReplaceSet;

void PrettifyLine(std::string& s) {
  // do replacement for syntactical pairs
  for (const auto& p_replace : kSyntaxReplaceSet) {
    auto cmp = s.find(p_replace.first);
    if (cmp != string::npos) {
      s.replace(cmp, p_replace.first.length(), p_replace.second);
    }
  }

  // do replacement for normal variables
  for (const auto& p_replace : NormVarReplaceSet) {
    auto cmp = s.find(p_replace.first);
    if (cmp != string::npos) {
      s.replace(cmp, p_replace.first.length(), p_replace.second);
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

  // do replacement for path variables
  for (const auto& p_replace : LotusVarReplaceSet) {
    auto cmp = s.find(p_replace.first);
    if (cmp != string::npos) {
      s.replace(cmp, p_replace.first.length(), p_replace.second);
    }
  }
}
}  // namespace

Packages::Packages(string n, unique_ptr<ifstream> ifs, string prettify_filename)
    : ifs_(std::move(ifs)), filename_(std::move(n)), headers_(make_unique<map<string, unsigned int>>()) {
  if (!*ifs_) {
    throw std::runtime_error("Cannot open file");
  }

  Timer t;
  t.Start();

  ParseFile(ifs_.get());

  // initialize constant replacement maps
  kSyntaxReplaceSet = {
      {"=", ": "},
      {"{}", "(empty hash)"},
      {"[]", "(empty array)"},
      {"\"\"", "(empty string)"}
  };
  kBoolReplaceSet = {
      {"0", "false"},
      {"1", "true"}
  };

  // replace with default path if no file is specified for prettify
  if (prettify_filename.empty()) {
    Log::d("No prettify file specified. Using default path.");
    prettify_filename = "prettify.txt";
  }

  Log::i("Using prettify source \"" + prettify_filename + "\"");

  // read the file and interpret
  ConfigFile cf(std::move(prettify_filename));
  if (cf.ReadFromFile()) {
    Log::d("Prettify file is valid");

    // [normal]
    try {
      const auto& norm_replace = cf.GetSection("normal");
      NormVarReplaceSet.clear();
      for (auto&& set : norm_replace) {
        NormVarReplaceSet.emplace(set.first, set.second);
      }
    } catch (std::runtime_error& rt_ex) {
      Log::w("Section \"normal\" not found. Will not replace normal fields");

      // no need to handle it
    }

    // [bool]
    try {
      const auto& bool_replace = cf.GetSection("bool");
      BoolVarReplaceSet.clear();
      for (auto&& set : bool_replace) {
        BoolVarReplaceSet.emplace(set.first, set.second);
      }
    } catch (std::runtime_error& rt_ex) {
      Log::w("Section \"bool\" not found. Will not replace boolean fields");

      // no need to handle it
    }

    // [lotus]
    try {
      const auto& lotus_replace = cf.GetSection("lotus");
      LotusVarReplaceSet.clear();
      for (auto&& set : lotus_replace) {
        LotusVarReplaceSet.emplace(set.first, set.second);
      }
    } catch (std::runtime_error& rt_ex) {
      Log::w("Section \"lotus\" not found. Will not replace item fields");

      // no need to handle it
    }

  } else {
    Log::d("Prettify file is invalid. Will not replace fields.");
  }

  t.Stop();
  auto time = static_cast<unsigned int>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::i("Initialization of Packages(\"" + filename_ + "\") complete. Took " + std::to_string(time) + "ms.");
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

  Log::v("Dumping data for " + header);
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

  Log::v("Data dump complete");
  Log::FlushFileBuf();
}

void Packages::Find(std::string header, bool search_front, unsigned int max_size) {
  std::transform(header.begin(), header.end(), header.begin(), ::tolower);
  auto matches = make_unique<vector<string>>();

  Log::d("Start search for \"" + header + "\" in header substrings");

  Timer t;
  t.Start();

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

  t.Stop();
  auto time = static_cast<unsigned int>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Search complete. Took " + std::to_string(time) + "ms.");
  Log::d("Found " + std::to_string(matches->size()) + " matches.");

  // check if we have more matches than max_size
  if (matches->size() > max_size) {
    cout << "Display all " << matches->size() << " possibilities? (y/n) ";
    string response;
    getline(cin, response);
    if (response != "y" && response != "Y") {
      Log::i("Skip displaying matches from user input");
      Log::FlushFileBuf();
      return;
    }
  }

  // display all matches and total count
  for (auto&& e : *matches) {
    cout << e << '\n';
  }
  cout << endl;
  cout << matches->size() << " entries." << endl;

  Log::FlushFileBuf();
}

void Packages::Compare(std::string cmp_filename) {
  Log::i("Packages::Compare(...): " + filename_ + " <-> " + cmp_filename);

  auto cmp_filestream = make_unique<ifstream>(cmp_filename);
  if (!*cmp_filestream) {
    cout << cmp_filename << ": File not found." << endl;
    return;
  }
  auto cmp_file = make_unique<Packages>(cmp_filename, std::move(cmp_filestream));

  auto has_current = make_unique<vector<string>>();
  auto has_compare = make_unique<vector<string>>();

  const auto cmp_file_headers = cmp_file->GetHeaderPtr();

  Log::d("Begin header comparison");

  Timer t;
  t.Start();

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

  t.Stop();
  auto time = static_cast<unsigned int>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Comparison complete. Took " + std::to_string(time) + "ms.");

  system("cls");

  if (!has_current->empty()) {
    cout << "Headers which only exist in current version: " << endl;
    for (auto&& h : *has_current) {
      cout << h << '\n';
    }
    cout << endl;
  }

  if (!has_compare->empty()) {
    cout << "Headers which only exist in comparing version: " << endl;
    for (auto&& h : *has_compare) {
      cout << h << '\n';
    }
    cout << endl;
  }

  if (!has_current->empty() || !has_compare->empty()) {
    cout << has_compare->size() << " additions, " << has_current->size() << " deletions" << endl;
  } else {
    cout << "Headers are identical." << endl;
  }

  Log::FlushFileBuf();
}

void Packages::SortFile(string outfile, unsigned int notify_count) {
  Log::i("Packages::SortFile -> " + outfile);

  // initialize variables
  auto instream = make_unique<ifstream>(filename_);
  auto contents = make_unique<map<string, vector<string>>>();
  auto outstream = make_unique<ofstream>(outfile);

  cout << "Loading file, please wait..." << endl;

  Log::d("Begin full file load");

  Timer t;
  t.Start();

  // re-read the whole file into RAM
  string category;
  string buffer_line;
  for (auto i = 0; getline(*instream, buffer_line); ++i) {
    if (buffer_line.empty()) {
      continue;
    }
    auto is_category = buffer_line.substr(0, 17) == "~FullPackageName=";
    if (is_category) {
      category = buffer_line.substr(17);
      contents->emplace(category, vector<string>());
      contents->at(category).emplace_back(buffer_line);
    } else if (category.empty()) {
      continue;
    } else {
      ConvertTabToSpace(buffer_line);
      contents->at(category).emplace_back(buffer_line);
    }
  }

  t.Stop();
  auto time = static_cast<unsigned int>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Read complete. Took " + std::to_string(time) + "ms.");

  instream->close();

  t.Reset();

  unsigned count{0};
  const auto total = contents->size();

  Log::d("Begin full file dump");

  t.Start();

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

  t.Stop();
  time = static_cast<unsigned int>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Dump complete. Took " + std::to_string(time) + "ms.");

  outstream->close();
  Log::FlushFileBuf();
}

void Packages::ReverseLookup(unsigned int line, bool is_interactive) {
  auto rev_headers = make_unique<map<unsigned int, string>>();

  Log::d("Reversing header map...");

  Timer t;
  t.Start();

  cout << "Loading..." << endl;

  for (auto&& a : *headers_) {
    rev_headers->emplace(a.second, a.first);
  }

  t.Stop();
  auto time = static_cast<unsigned int>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Reversal complete. Took " + std::to_string(time) + "ms.");

  t.Reset();

  Log::d("Searching for line...");

  t.Start();

  auto i = rev_headers->end();
  for (auto it = rev_headers->begin(); it != rev_headers->end(); ++it) {
    if (it->first + 1 > line) {
      i = --it;
      break;
    }
  }

  t.Stop();
  time = static_cast<unsigned int>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Search complete. Took " + std::to_string(time) + "ms.");

  system("cls");

  if (i != rev_headers->end()) {
    cout << "Entry at line " << line << ": " << i->second << endl;
    cout << "Entry begins at line " << i->first + 1 << endl << endl;
    if (is_interactive) {
      cout << "View Package Details? [y/N] ";
      string resp;
      getline(cin, resp);
      if (resp == "y" || resp == "Y") {
        OutputHeader(i->second, false);
      }
    }
  } else {
    Log::w("Line " + std::to_string(line) + " has no entry");
    cout << "No entry found at line " << line << endl << endl;
  }

}

void Packages::ParseFile(ifstream* ifs) {
  Log::d("Packages::ParseFile");

  cout << "Reading file, please wait..." << endl;
  string buffer_line;

  // read file and save with line numbers
  for (auto i = 0; getline(*ifs, buffer_line); ++i) {
    if (buffer_line.empty()) {
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
  Log::d("Packages::GetHeaderContents(" + header + ")");

  auto content = make_unique<vector<string>>();

  auto search = headers_->find(header);
  if (search == headers_->end()) {
    Log::w("Cannot find header!");
    return content;
  }

  auto index = search->second + 1 + static_cast<unsigned>(!inc_header);

  Log::v("Packages::GetHeaderContents: Will start reading from line " + std::to_string(index));

  auto fs = GotoLine(index);

  string line;
  for (auto it = index; getline(*fs, line); ++it) {
    if (it != index && line.substr(0, 17) == "~FullPackageName=") {
      Log::v("Packages::GetHeaderContents: Next header found. Breaking.");
      break;
    }
    ConvertTabToSpace(line);
    content->push_back(line);
  }

  return content;
}
