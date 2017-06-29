// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities to parse a legacy-format Packages file.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_PACKAGES_LEGACY_H_
#define WARFRAME_PACKAGES_DEPARSER_PACKAGES_LEGACY_H_

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

class PackagesLegacy {
 public:
  PackagesLegacy(std::string filename, std::unique_ptr<std::ifstream> ifs);

  void OutputHeaderRaw(std::string header);
  void DeparseHeader(std::string header);

  void SuggestSimilar(std::string header, unsigned int max_size = 10);

  auto GetFilename() const -> std::string { return filename_; }
  auto GetSize() const -> std::size_t { return header_contents_->size(); }
 private:
  void ParseFile(std::ifstream*);
  auto GotoLine(std::ifstream& file, unsigned int line) -> std::ifstream&;

  std::string filename_ = "";
  std::unique_ptr<std::map<std::string, std::vector<std::string>>> header_contents_ = nullptr;
  std::unique_ptr<std::map<std::string, unsigned int>> header_line_ = nullptr;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_PACKAGES_LEGACY_H_
