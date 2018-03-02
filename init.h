// Copyright (c) 2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Initialization methods.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_INIT_H_
#define WARFRAME_PACKAGES_DEPARSER_INIT_H_

#include <string>

const std::string kBuildString = "0.11.0-rc.2";

void Init();
void OutputHelp(const std::string& s);
void OutputVersionInfo();

#endif  // WARFRAME_PACKAGES_DEPARSER_INIT_H_
