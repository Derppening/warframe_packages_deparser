// Copyright (c) 2017-2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for Packages class.
//

#include "packages.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "config_file.h"
#include "log.h"
#include "prettify.h"
#include "timer.h"
#include "util.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;

/**
 * @brief Packages constructor.
 *
 * @param filename Input filename
 * @param ifs Input file stream
 * @param prettify_filename Prettify filename
 */
Packages::Packages(const std::string& filename, std::ifstream&& ifs, std::string prettify_filename)
    : ifs_(std::move(ifs)), filename_(filename), headers_(std::map<std::string, unsigned>()) {
  if (!ifs_) {
    throw std::runtime_error("Cannot open file");
  }

  Timer t;
  t.Start();

  ParseFile(&ifs_);

  // replace with default path if no file is specified for prettify
  if (prettify_filename.empty()) {
    Log::d("No prettify file specified. Using default path.");
    prettify_filename = "prettify.txt";
  }

  Log::i("Using prettify source \"" + prettify_filename + "\"");

  ParsePrettify(prettify_filename);

  t.Stop();
  auto time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());

  Log::i("Initialization of Packages(\"" + filename_ + "\") complete. Took " + std::to_string(time) + "ms.");
}
