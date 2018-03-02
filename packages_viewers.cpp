// Copyright (c) 2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for view-only functions of Packages class.
//

#include "packages.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "log.h"
#include "prettify.h"
#include "timer.h"
#include "util.h"

using std::cin;
using std::cout;
using std::endl;

/**
 * @brief Outputs the contents of a given header.
 *
 * @param header Target header
 * @param is_raw Whether to output the contents in the raw format
 */
void Packages::OutputHeader(const std::string& header, bool is_raw) {
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

/**
 * @brief Find all headers containing the given string.
 *
 * @param header String to match
 * @param search_front If true, only return headers which starts from header
 * @param max_size Maximum matches before the application prompts the user for input.
 */
void Packages::Find(std::string&& header, bool search_front, unsigned max_size) {
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

/**
 * @brief Compare the contents between the loaded file and another file.
 *
 * @param cmp_filename Filename of the comparing file
 */
void Packages::Compare(const std::string& cmp_filename) {
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

/**
 * @brief Lookup the header based on the line number.
 *
 * @param line Line number to lookup
 * @param is_interactive If true, will prompt user if they want to view the header contents
 */
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
