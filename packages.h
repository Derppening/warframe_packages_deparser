// Copyright (c) 2017-2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities to parse a current-format Packages file.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_PACKAGES_H_
#define WARFRAME_PACKAGES_DEPARSER_PACKAGES_H_

#include <fstream>
#include <map>
#include <string>
#include <vector>

class Packages {
 public:
  enum struct SortOptions : unsigned {
    /**
     * @brief Whether to output the diff-optimized version.
     */
    kDiff = 1 << 0,
    /**
     * @brief Whether to output the prettified version.
     */
    kPrettify = 1 << 1
  };

  enum struct StructureOptions {
    /**
     * @brief Do not dump JSON.
     */
    kNone,
    /**
     * @brief Dump JSON with C++ scope-like syntax.
     */
    kScope,
    /**
     * @brief Dump JSON with a tree-like format.
     */
    kTree
  };

  Packages(const std::string& filename, std::ifstream&& ifs, std::string&& prettify_filename = "");

  void OutputHeader(const std::string& header, bool is_raw);

  void Find(std::string&& header, bool search_front, unsigned max_size);

  void Compare(const std::string& cmp_filename);

  void SortFile(const std::string& outfile, unsigned opt_mask, unsigned notify_count);

  void ReverseLookup(unsigned line, bool is_interactive);

  std::vector<std::string> HeaderToJson(const std::string& header, StructureOptions opts, std::vector<std::string>&& read_file);
  void DumpJson(std::string&& outfile, unsigned notify_count);

  auto GetFilename() const -> std::string { return filename_; }
  auto GetSize() const -> std::size_t { return headers_.size(); }

 private:
  void ParseFile(std::ifstream* ifs);

  auto GetHeaderContents(const std::string& header, bool inc_header = false) -> std::vector<std::string>;

  std::ifstream ifs_;
  std::string filename_ = "";
  std::map<std::string, unsigned> headers_;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_PACKAGES_H_
