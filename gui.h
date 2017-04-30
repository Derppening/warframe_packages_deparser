//
// Created by david on 30/4/2017.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_GUI_H_
#define WARFRAME_PACKAGES_DEPARSER_GUI_H_

#include <memory>

#include "packages.h"

class Gui {
 public:
  Gui(Packages& package);
  void MainMenu();

 private:
  Packages* package_;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_GUI_H_
