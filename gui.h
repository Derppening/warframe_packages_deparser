//
// Created by david on 30/4/2017.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_GUI_H_
#define WARFRAME_PACKAGES_DEPARSER_GUI_H_

#include <memory>

#include "packages.h"
#include "packages_legacy.h"

class Gui {
 public:
  enum struct PackageVer {
    kLegacy = 0,
    kCurrent = 1
  };

  Gui(Packages& package);
  Gui(PackagesLegacy& package);
  Gui(Packages* package);
  Gui(PackagesLegacy* package);

  void MainMenu();

  std::string GetFileName() const;
  std::size_t GetSize() const;

  void Find(const std::string substr, const unsigned int max_size) const;
  void FindFront(const std::string header, const unsigned int max_size) const;
  void OutputHeader(const std::string header) const;
  void OutputHeaderRaw(const std::string header) const;

 private:
  Packages* packages_ = nullptr;
  PackagesLegacy* package_legacy_ = nullptr;

  PackageVer package_ver_ = PackageVer::kCurrent;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_GUI_H_
