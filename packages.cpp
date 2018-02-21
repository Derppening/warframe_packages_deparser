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

/**
 * @brief Sort the file lexicographically.
 *
 * @param outfile Filename of the output
 * @param opt_mask Bit mask of options to apply. See @c SortOptions
 * @param notify_count How often to output progress
 */
void Packages::SortFile(std::string outfile, unsigned opt_mask, unsigned notify_count) {
  Log::i("Packages::SortFile -> " + outfile);

  // initialize variables
  auto instream = std::ifstream(filename_);
  auto contents = std::map<std::string, std::vector<std::string>>();
  auto outstream = std::ofstream(outfile);

  cout << "Loading file, please wait..." << endl;

  Log::d("Begin full file load");

  Timer t;
  t.Start();

  // re-read the whole file into RAM
  std::string category;
  std::string buffer_line;
  for (auto i = 0; getline(instream, buffer_line); ++i) {
    if (buffer_line.empty()) {
      continue;
    }

    // remove trailing CR character in *nix systems
    if (buffer_line.back() == '\r') {
      buffer_line.pop_back();
    }

    // sort the contents based on what header they lie under
    auto start_of_category = buffer_line.find("FullPackageName=");
    if (start_of_category != std::string::npos) {
      category = buffer_line.substr(start_of_category + 16);
      contents.emplace(category, std::vector<std::string>());
      if ((opt_mask & static_cast<unsigned>(SortOptions::kDiff)) && buffer_line.front() == '~') {
        buffer_line.erase(0, 1);
      }
      contents.at(category).emplace_back(buffer_line);
    } else if (category.empty()) {
      continue;
    } else {
      ConvertTabToSpace(buffer_line);
      if ((opt_mask & static_cast<unsigned>(SortOptions::kDiff)) && buffer_line.substr(0, 12) == "BasePackage=") {
        buffer_line.insert(0, "  ");
      }
      contents.at(category).emplace_back(buffer_line);
    }
  }

  t.Stop();
  auto time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());
  Log::d("Read complete. Took " + std::to_string(time) + "ms.");
  t.Reset();

  instream.close();

  unsigned count{0};
  const auto total = contents.size();

  Log::d("Begin full file dump");

  t.Start();

  // dump map into new file
  for (auto&& p : contents) {
    if (++count % notify_count == 0) {
      cout << "Dumping header: " << count << "/" << total << endl;
    }
    for (auto&& l : p.second) {
      if (opt_mask & static_cast<unsigned>(SortOptions::kPrettify)) {
        PrettifyLine(l);
      }

      outstream << l << '\n';
    }
    outstream.flush();
  }

  t.Stop();
  time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());
  Log::d("Dump complete. Took " + std::to_string(time) + "ms.");

  outstream.close();
  Log::FlushFileBuf();
}
