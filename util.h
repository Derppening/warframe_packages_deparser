// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities which are used to aid the program
//

#ifndef WARFRAME_PACKAGES_DEPARSER_UTIL_H_
#define WARFRAME_PACKAGES_DEPARSER_UTIL_H_

#include <string>
#include <vector>

auto SplitString(std::string input, std::string delimiter, unsigned limit = 0) -> std::vector<std::string>;

#endif  // WARFRAME_PACKAGES_DEPARSER_UTIL_H_
