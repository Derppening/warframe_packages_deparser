// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for util.h
//

#include "util.h"

#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using std::size_t;
using std::string;
using std::vector;

auto SplitString(string input, string delimiter, unsigned limit) -> vector<string> {
  size_t pos = 0;
  unsigned count = 0;
  string token;
  vector<string> vs;

  while ((pos = input.find(delimiter)) != string::npos) {
    vs.emplace_back(input.substr(0, pos));
    input.erase(0, pos + delimiter.length());
    ++count;
    if (limit != 0 && count >= limit) {
      return vs;
    }
  }

  if (input.length() != 0) {
    vs.emplace_back(input);
  }

  return vs;
}

auto JoinToString(const std::vector<std::string>& input, std::string separator) -> std::string {
  std::stringstream arg;
  std::copy(input.begin(), input.end(), std::ostream_iterator<std::string>(arg, separator.c_str()));
  return arg.str();
}
