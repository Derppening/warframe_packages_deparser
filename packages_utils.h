// Copyright (c) 2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities to assist Package-parsing.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_PACKAGES_UTILS_H_
#define WARFRAME_PACKAGES_DEPARSER_PACKAGES_UTILS_H_

#include <map>
#include <string>

void ParsePrettify(const std::string& filename);
void PrettifyLine(std::string& s);

#endif  // WARFRAME_PACKAGES_DEPARSER_PACKAGES_UTILS_H_
