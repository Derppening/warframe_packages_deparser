// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities to parse a current-format Packages file.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_PACKAGES_H_
#define WARFRAME_PACKAGES_DEPARSER_PACKAGES_H_

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Packages {
 public:
  Packages(const std::string& filename, std::ifstream&& ifs, std::string prettify_filename = "");

  void OutputHeader(std::string header, bool is_raw);

  void Find(std::string header, bool search_front, unsigned max_size);

  void Compare(std::string);

  void SortFile(std::string, bool, unsigned);

  void ReverseLookup(unsigned, bool);

  auto GetFilename() const -> std::string { return filename_; }
  auto GetSize() const -> std::size_t { return headers_.size(); }

 private:
  void ParseFile(std::ifstream* ifs);
  auto GotoLine(unsigned line) -> std::ifstream;
  void ConvertTabToSpace(std::string& str);

  auto GetHeaderContents(std::string header, bool inc_header = false) -> std::vector<std::string>;

  std::ifstream ifs_;
  std::string filename_ = "";
  std::map<std::string, unsigned> headers_;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_PACKAGES_H_
