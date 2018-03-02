// Copyright (c) 2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for utility functions of Packages class.
//

#include "packages.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "log.h"
#include "util.h"

using std::cout;
using std::endl;

/**
 * @brief Parses the input file, saves all headers and their corresponding line number.
 *
 * @param ifs Input file stream
 */
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

/**
 * @brief Retrieves the contents of a header.
 *
 * @param header Header to retrieve the contents
 * @param inc_header Whether to include the header line
 *
 * @return All lines of the given header
 */
auto Packages::GetHeaderContents(const std::string& header, bool inc_header) -> std::vector<std::string> {
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

