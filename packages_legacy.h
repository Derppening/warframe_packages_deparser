//
// Created by david on 30/4/2017.
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

  std::string GetFilename() const { return filename_;}
  std::size_t GetSize() const { return header_contents_->size(); }
 private:
  void ParseFile(std::ifstream*);
  std::ifstream& GotoLine(std::ifstream& file, unsigned int line);

  std::string filename_ = "";
  std::unique_ptr<std::map<std::string, std::vector<std::string>>> header_contents_ = nullptr;
  std::unique_ptr<std::map<std::string, unsigned int>> header_line_ = nullptr;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_PACKAGES_LEGACY_H_
