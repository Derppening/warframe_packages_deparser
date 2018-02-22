// Copyright (c) 2017-2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Initializes the console user interface, and acts as the gateway to the
// program's functionalities.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_GUI_H_
#define WARFRAME_PACKAGES_DEPARSER_GUI_H_

#include <memory>

#include "packages.h"

class Gui {
 public:
  enum struct PackageVer {
    kCurrent
  };

  enum struct GuiOption {
    kExit,
    kHelp,
    kFind,
    kView,
    kSort,
    kCompare,
    kDumpJson,
    kNoOpt
  };

  explicit Gui(Packages* package);

  void MainMenu();

  void Help(bool is_interactive) const;
  void Find(std::string args, bool is_interactive) const;
  void View(std::string args) const;
  void Sort(std::string args) const;
  void Compare(std::string args) const;
  void Json(const std::string args) const;

 private:
  auto GetFileName() const -> std::string;
  auto GetSize() const -> std::size_t;

  Packages* packages_ = nullptr;

  PackageVer package_ver_ = PackageVer::kCurrent;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_GUI_H_
