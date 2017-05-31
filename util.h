//
// Created by david on 31/5/2017.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_UTIL_H_
#define WARFRAME_PACKAGES_DEPARSER_UTIL_H_

#include <string>
#include <vector>

std::vector<std::string> SplitString(std::string input,
                                     std::string delimiter,
                                     unsigned limit = 0);

std::vector<std::string> SplitString(std::string input,
                                     char delimiter,
                                     unsigned limit = 0);

bool Require(bool predicate, std::string message, bool is_fatal = false);

#endif  // WARFRAME_PACKAGES_DEPARSER_UTIL_H_
