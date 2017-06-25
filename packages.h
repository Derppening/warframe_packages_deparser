//
// Created by david on 3/5/2017.
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
  Packages(std::string filename, std::unique_ptr<std::ifstream> ifs);

  void OutputHeader(std::string header);
  void OutputHeaderRaw(std::string header);

  void Find(std::string header, bool search_front, unsigned int max_size);

  void Compare(std::string);

  void SortFile(std::string, unsigned int);

  std::string GetFilename() const { return filename_; }
  const std::map<std::string, unsigned int>* GetHeaderPtr() const { return headers_.get(); }
  std::size_t GetSize() const { return headers_->size(); }

 private:
  void ParseFile(std::ifstream *ifs);
  std::unique_ptr<std::ifstream> GotoLine(unsigned int line);
  void ConvertTabToSpace(std::string& str);

  std::unique_ptr<std::vector<std::string>> GetHeaderContents(std::string header, bool inc_header = false);

  std::unique_ptr<std::ifstream> ifs_ = nullptr;
  std::string filename_ = "";
  std::unique_ptr<std::map<std::string, unsigned int>> headers_ = nullptr;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_PACKAGES_H_
