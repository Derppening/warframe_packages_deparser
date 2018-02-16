// Copyright (c) 2017-2018 David Mak. All rights reserved.
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
#include "prettify.h"
#include "timer.h"
#include "util.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;

Packages::Packages(const std::string& n, std::ifstream&& ifs, std::string prettify_filename)
    : ifs_(std::move(ifs)), filename_(n), headers_(std::map<std::string, unsigned>()) {
  if (!ifs_) {
    throw std::runtime_error("Cannot open file");
  }

  Timer t;
  t.Start();

  ParseFile(&ifs_);

  // replace with default path if no file is specified for prettify
  if (prettify_filename.empty()) {
    Log::d("No prettify file specified. Using default path.");
    prettify_filename = "prettify.txt";
  }

  Log::i("Using prettify source \"" + prettify_filename + "\"");

  ParsePrettify(prettify_filename);

  t.Stop();
  auto time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::i("Initialization of Packages(\"" + filename_ + "\") complete. Took " + std::to_string(time) + "ms.");
}

void Packages::OutputHeader(std::string header, bool is_raw) {
  // find the header. return if we can't find it
  auto search = headers_.find(header);
  if (search == headers_.end()) {
    cout << header << ": Header not found." << endl;
    return;
  }

  ClearScreen();

  // read from file first. we need the base package
  cout << "Loading entry..." << endl;
  std::vector<std::string> contents = GetHeaderContents(header);
  ClearScreen();

  Log::v("Dumping data for " + header);
  if (is_raw) {
    // provide some raw information
    cout << "Package Name: " << header << endl;
    cout << "Line Number in File: " << search->second + 1 << endl;
    cout << endl;

    // display the contents
    for (auto&& l : contents) {
      cout << l << endl;
    }
  } else {
    // provide some raw information
    cout << "Package Name: " << header << endl;
    if (contents[0].find("BasePackage=") != std::string::npos) {
      cout << "Base Package: " << contents[0].substr(contents[0].find("BasePackage=") + 12) << endl;
      contents.erase(contents.begin());
    }
    cout << endl;
    cout << "Line Number in File: " << search->second + 1 << endl;
    cout << endl;

    // display the contents
    for (auto& it : contents) {
      PrettifyLine(it);

      cout << it << endl;
    }
  }

  Log::v("Data dump complete");
  Log::FlushFileBuf();
}

void Packages::Find(std::string header, bool search_front, unsigned max_size) {
  std::transform(header.begin(), header.end(), header.begin(), ::tolower);
  auto matches = std::vector<std::string>();

  Log::d("Start search for \"" + header + "\" in header substrings");

  Timer t;
  t.Start();

  // find all matches
  for (auto&& p : headers_) {
    std::string s{p.first};
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (search_front && s.substr(0, header.size()) == header) {
      matches.emplace_back(p.first);
    } else if (!search_front && s.find(header) != std::string::npos) {
      matches.emplace_back(p.first);
    }
  }

  t.Stop();
  auto time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Search complete. Took " + std::to_string(time) + "ms.");
  Log::d("Found " + std::to_string(matches.size()) + " matches.");

  // check if we have more matches than max_size
  if (matches.size() > max_size) {
    cout << "Display all " << matches.size() << " possibilities? (y/n) ";
    std::string response;
    getline(cin, response);
    if (response != "y" && response != "Y") {
      Log::i("Skip displaying matches from user input");
      Log::FlushFileBuf();
      return;
    }
  }

  // display all matches and total count
  for (auto&& e : matches) {
    cout << e << '\n';
  }
  cout << endl;
  cout << matches.size() << " entries." << endl;

  Log::FlushFileBuf();
}

void Packages::Compare(std::string cmp_filename) {
  Log::i("Packages::Compare(...): " + filename_ + " <-> " + cmp_filename);

  auto cmp_filestream = std::ifstream(cmp_filename);
  if (!cmp_filestream) {
    cout << cmp_filename << ": File not found." << endl;
    return;
  }
  auto cmp_file = std::make_unique<Packages>(cmp_filename, std::move(cmp_filestream));

  auto has_current = std::vector<std::string>();
  auto has_compare = std::vector<std::string>();

  const std::map<std::string, unsigned>& cmp_file_headers = cmp_file->headers_;

  Log::d("Begin header comparison");

  Timer t;
  t.Start();

  cout << "Searching for new keys in current file..." << endl;
  for (auto&& h : headers_) {
    if (cmp_file_headers.find(h.first) == cmp_file_headers.cend()) {
      has_current.emplace_back(h.first);
    }
  }

  cout << "Searching for new keys in comparing file..." << endl;
  for (auto&& h : cmp_file_headers) {
    if (headers_.find(h.first) == headers_.cend()) {
      has_compare.emplace_back(h.first);
    }
  }

  t.Stop();
  auto time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Comparison complete. Took " + std::to_string(time) + "ms.");

  ClearScreen();

  if (!has_current.empty()) {
    cout << "Headers which only exist in current version: " << endl;
    for (auto&& h : has_current) {
      cout << h << '\n';
    }
    cout << endl;
  }

  if (!has_compare.empty()) {
    cout << "Headers which only exist in comparing version: " << endl;
    for (auto&& h : has_compare) {
      cout << h << '\n';
    }
    cout << endl;
  }

  if (!has_current.empty() || !has_compare.empty()) {
    cout << has_current.size() << " additions, " << has_compare.size() << " deletions" << endl;
  } else {
    cout << "Headers are identical." << endl;
  }

  Log::FlushFileBuf();
}

void Packages::SortFile(std::string outfile, unsigned opt_mask, unsigned notify_count) {
  Log::i("Packages::SortFile -> " + outfile);

  // initialize variables
  auto instream = std::ifstream(filename_);
  auto contents = std::map<std::string, std::vector<std::string>>();
  auto outstream = std::ofstream(outfile);

  cout << "Loading file, please wait..." << endl;

  Log::d("Begin full file load");

  Timer t;
  t.Start();

  // re-read the whole file into RAM
  std::string category;
  std::string buffer_line;
  for (auto i = 0; getline(instream, buffer_line); ++i) {
    if (buffer_line.empty()) {
      continue;
    }

    // remove trailing CR character in *nix systems
    if (buffer_line.back() == '\r') {
      buffer_line.pop_back();
    }

    // sort the contents based on what header they lie under
    auto start_of_category = buffer_line.find("FullPackageName=");
    if (start_of_category != std::string::npos) {
      category = buffer_line.substr(start_of_category + 16);
      contents.emplace(category, std::vector<std::string>());
      if ((opt_mask & static_cast<unsigned>(SortOptions::kDiff)) && buffer_line.front() == '~') {
        buffer_line.erase(0, 1);
      }
      contents.at(category).emplace_back(buffer_line);
    } else if (category.empty()) {
      continue;
    } else {
      ConvertTabToSpace(buffer_line);
      if ((opt_mask & static_cast<unsigned>(SortOptions::kDiff)) && buffer_line.substr(0, 12) == "BasePackage=") {
        buffer_line.insert(0, "  ");
      }
      contents.at(category).emplace_back(buffer_line);
    }
  }

  t.Stop();
  auto time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());
  Log::d("Read complete. Took " + std::to_string(time) + "ms.");
  t.Reset();

  instream.close();

  unsigned count{0};
  const auto total = contents.size();

  Log::d("Begin full file dump");

  t.Start();

  // dump map into new file
  for (auto&& p : contents) {
    if (++count % notify_count == 0) {
      cout << "Dumping header: " << count << "/" << total << endl;
    }
    for (auto&& l : p.second) {
      outstream << l << '\n';
    }
    outstream.flush();
  }

  t.Stop();
  time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());
  Log::d("Dump complete. Took " + std::to_string(time) + "ms.");

  outstream.close();
  Log::FlushFileBuf();
}

void Packages::ReverseLookup(unsigned line, bool is_interactive) {
  auto rev_headers = std::map<unsigned, std::string>();

  Log::d("Reversing header map...");

  Timer t;
  t.Start();

  cout << "Loading..." << endl;

  std::for_each(headers_.begin(), headers_.end(), [&rev_headers](const std::pair<std::string, unsigned>& a) {
    rev_headers.emplace(a.second, a.first);
  });

  t.Stop();
  auto time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Reversal complete. Took " + std::to_string(time) + "ms.");

  t.Reset();

  Log::d("Searching for line...");

  t.Start();

  auto i = rev_headers.end();
  for (auto it = rev_headers.begin(); it != rev_headers.end(); ++it) {
    if (it->first + 1 > line) {
      i = --it;
      break;
    }
  }

  t.Stop();
  time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::d("Search complete. Took " + std::to_string(time) + "ms.");

  ClearScreen();

  if (i != rev_headers.end()) {
    cout << "Entry at line " << line << ": " << i->second << endl;
    cout << "Entry begins at line " << i->first + 1 << endl << endl;
    if (is_interactive) {
      cout << "View Package Details? [y/N] ";
      std::string resp;
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

void Packages::ParseFile(std::ifstream* const ifs) {
  Log::d("Packages::ParseFile");

  cout << "Reading file, please wait..." << endl;
  std::string buffer_line;

  // read file and save with line numbers
  for (unsigned i = 0; getline(*ifs, buffer_line); ++i) {
    if (buffer_line.empty()) {
      continue;
    }

    // remove trailing CR character in *nix systems
    if (buffer_line.back() == '\r') {
      buffer_line.pop_back();
    }

    auto start_of_category = buffer_line.find("FullPackageName=");
    if (start_of_category != std::string::npos) {
      std::string category = buffer_line.substr(start_of_category + 16);
      headers_.emplace(category, i);
    }
  }
}

auto Packages::GetHeaderContents(std::string header, bool inc_header) -> std::vector<std::string> {
  Log::d("Packages::GetHeaderContents(" + header + ")");

  auto content = std::vector<std::string>();

  auto search = headers_.find(header);
  if (search == headers_.end()) {
    Log::w("Cannot find header!");
    return content;
  }

  unsigned index = search->second + 1 + static_cast<unsigned>(!inc_header);

  Log::v("Packages::GetHeaderContents: Will start reading from line " + std::to_string(index));

  auto fs = std::ifstream(filename_);
  GotoLine(fs, index);

  std::string line;
  for (unsigned it = index; getline(fs, line); ++it) {
    if (it != index && line.find("FullPackageName=") != std::string::npos) {
      Log::v("Packages::GetHeaderContents: Next header found. Breaking.");
      break;
    }

    // remove trailing CR character in *nix systems
    if (line.back() == '\r') {
      line.pop_back();
    }

    ConvertTabToSpace(line);
    content.push_back(line);
  }

  return content;
}
