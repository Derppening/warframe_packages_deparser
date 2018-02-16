// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for util.h
//

#include "util.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>

using std::size_t;

/**
 * @brief Splits a string by a given delimiter.
 *
 * @param input Input string
 * @param delimiter Delimiter string
 * @param limit Maximum number of substrings to return. 0 implies no limit.
 * @return Vector of substrings after split
 */
auto SplitString(std::string input, std::string delimiter, unsigned limit) -> std::vector<std::string> {
  size_t pos = 0;
  unsigned count = 0;
  std::string token;
  std::vector<std::string> vs;

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

/**
 * @brief Joins a string from a vector using a given separator.
 *
 * @param input Input vector of strings
 * @param separator Separator between the strings
 * @return Joined string
 */
auto JoinToString(const std::vector<std::string>& input, std::string separator) -> std::string {
  std::stringstream arg;
  std::copy(input.begin(), input.end(), std::ostream_iterator<std::string>(arg, separator.c_str()));
  return arg.str();
}

/**
 * @brief Jumps to a certain line in an open @c std::ifstream.
 *
 * @param fs Input file stream
 * @param line Line number
 */
void GotoLine(std::ifstream& fs, unsigned line) {
  fs.seekg(std::ios::beg);

  // skip line number of lines
  for (unsigned it = 0; it < line - 1; ++it) {
    fs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

/**
 * @brief Converts tabs to spaces in a string.
 *
 * @param str String to be converted
 */
void ConvertTabToSpace(std::string& str) {
  std::string::size_type n = 0;
  while ((n = str.find('\t', n)) != std::string::npos) {
    str.replace(n, 1, "  ");
    n += 2;
  }
}

/**
 * @brief System-independent function for clearing a console screen.
 */
void ClearScreen() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  system("cls");
#else
  system("clear");
#endif  // defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
}
