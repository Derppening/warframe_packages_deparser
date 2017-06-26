//
// Created by david on 31/5/2017.
//

#include "util.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using std::size_t;
using std::string;
using std::vector;

auto SplitString(string input, string delimiter, unsigned limit) -> vector<string> {
  size_t pos{0};
  unsigned count{0};
  string token{};
  vector<string> vs{};

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
