// Copyright (c) 2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for Json-dumping functions of Packages class.
//

#include "packages.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "log.h"
#include "timer.h"
#include "util.h"

using std::cout;
using std::endl;

/**
 * @brief Converts a package file into JSON format.
 *
 * @param outfile File to output
 * @param notify_count How often to output progress
 */
void Packages::DumpJson(std::string&& outfile, unsigned notify_count) {
  Log::i("Packages::DumpJson -> " + outfile);

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
      contents.at(category).emplace_back(buffer_line);
    } else if (category.empty()) {
      continue;
    } else {
      ConvertTabToSpace(buffer_line);
      contents.at(category).emplace_back(buffer_line);
    }
  }

  t.Stop();
  auto time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());
  Log::d("Read complete. Took " + std::to_string(time) + "ms.");
  t.Reset();

  instream.close();

  unsigned count{0};
  unsigned failcount{0};
  const auto total = contents.size();

  Log::d("Begin full file dump");

  t.Start();

  // dump map into new file
  for (auto&& h : contents) {
    if (++count % notify_count == 0) {
      cout << "Dumping header: " << count << "/" << total << endl;
    }

    std::vector<std::string> json = HeaderToJson(h.first, StructureOptions::kNone, std::move(h.second));
    if (!json.empty()) {
      for (auto&& l : json) {
        outstream << l << '\n';
      }
      outstream.flush();
    } else {
      ++failcount;
    }
  }

  t.Stop();
  time = static_cast<unsigned>(std::chrono::duration_cast<Timer::milliseconds>(t.GetRawTime()).count());
  Log::d("Json dump complete. Took " + std::to_string(time) + "ms.");
  if (failcount != 0) {
    std::cerr << "Completed with " << failcount << " ignored packages." << std::endl;
    Log::w("Json dump resulted in " + std::to_string(failcount) + "errors.");
  }

  outstream.close();
  Log::FlushFileBuf();
}
