//
// Created by david on 30/4/2017.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_GUI_H_
#define WARFRAME_PACKAGES_DEPARSER_GUI_H_

#include <memory>

#include "packages_legacy.h"

class Gui {
 public:
  Gui(PackagesLegacy& package);
  void MainMenu();

 private:
  PackagesLegacy* package_;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_GUI_H_
