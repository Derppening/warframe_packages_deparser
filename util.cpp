//
// Created by david on 31/5/2017.
//

#include "util.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<std::string> SplitString(std::string input, std::string delimiter, unsigned limit) {
  std::size_t pos{0};
  unsigned count{0};
  std::string token{};
  std::vector<std::string> vs{};
  while ((pos = input.find(delimiter)) != std::string::npos) {
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

std::vector<std::string> SplitString(std::string input, char delimiter, unsigned limit) {
  return SplitString(input, std::string(1, delimiter), limit);
}

bool Require(bool predicate, std::string message, bool is_fatal) {
  if (predicate) {
    return true;
  }

  if (is_fatal) {
    throw std::invalid_argument(message);
  } else {
    std::cerr << message << std::endl;
  }
  return false;
}
