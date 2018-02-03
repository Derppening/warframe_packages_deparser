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
  Packages(std::string filename, std::unique_ptr<std::ifstream> ifs, std::string prettify_filename = "");

  void OutputHeader(std::string header, bool is_raw);

  void Find(std::string header, bool search_front, unsigned int max_size);

  void Compare(std::string);

  void SortFile(std::string, bool, unsigned int);

  void ReverseLookup(unsigned int, bool);

  auto GetFilename() const -> std::string { return filename_; }
  auto GetHeaderPtr() const -> const std::map<std::string, unsigned int>* { return headers_.get(); }
  auto GetSize() const -> std::size_t { return headers_->size(); }

 private:
  void ParseFile(std::ifstream *ifs);
  auto GotoLine(unsigned int line) -> std::unique_ptr<std::ifstream>;
  void ConvertTabToSpace(std::string& str);

  auto GetHeaderContents(std::string header, bool inc_header = false) -> std::unique_ptr<std::vector<std::string>>;

  std::unique_ptr<std::ifstream> ifs_ = nullptr;
  std::string filename_ = "";
  std::unique_ptr<std::map<std::string, unsigned int>> headers_ = nullptr;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_PACKAGES_H_
