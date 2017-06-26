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

  Gui(Packages* package);
  Gui(PackagesLegacy* package);

  void MainMenu();

 private:
  auto GetFileName() const -> std::string;
  auto GetSize() const -> std::size_t;

  void Find(const std::vector<std::string>& args) const;
  void View(const std::vector<std::string>& args) const;
  void Sort(const std::vector<std::string>& args) const;
  void Compare(const std::vector<std::string>& args) const;

  Packages* packages_ = nullptr;
  PackagesLegacy* package_legacy_ = nullptr;

  PackageVer package_ver_ = PackageVer::kCurrent;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_GUI_H_
