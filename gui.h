// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Initializes the console user interface, and acts as the gateway to the
// program's functionalities.
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
    kCurrent
  };

  enum struct GuiOption {
    kExit,
    kHelp,
    kFind,
    kView,
    kSort,
    kCompare,
    kNoOpt
  };

  Gui(Packages* package);
  Gui(PackagesLegacy* package);

  void MainMenu();

  bool ParseCommand(const std::vector<std::string>& args, bool is_interactive);

  void Find(const std::vector<std::string>& args, bool is_interactive) const;
  void View(const std::vector<std::string>& args) const;
  void Sort(const std::vector<std::string>& args) const;
  void Compare(const std::vector<std::string>& args) const;

 private:
  auto GetFileName() const -> std::string;
  auto GetSize() const -> std::size_t;

  Packages* packages_ = nullptr;
  PackagesLegacy* package_legacy_ = nullptr;

  PackageVer package_ver_ = PackageVer::kCurrent;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_GUI_H_
