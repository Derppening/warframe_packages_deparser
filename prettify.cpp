// Copyright (c) 2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for package utilities.
//

#include "prettify.h"

#include "config_file.h"
#include "log.h"

struct StrCompare {
  bool operator()(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) {
      return b.length() < a.length();
    }
    return a < b;
  }
};

std::map<std::string, std::string, StrCompare> NormVarReplaceSet;
std::map<std::string, std::string, StrCompare> BoolVarReplaceSet;
std::map<std::string, std::string, StrCompare> LotusVarReplaceSet;
std::map<std::string, std::string, StrCompare> kSyntaxReplaceSet = {
    {"=", ": "},
    {"{}", "(empty hash)"},
    {"[]", "(empty array)"},
    {"\"\"", "(empty string)"}
};
std::map<std::string, std::string, StrCompare> kBoolReplaceSet = {
    {"0", "false"},
    {"1", "true"}
};

void ParsePrettify(const std::string& filename) {
  ConfigFile cf(filename);
  if (cf.ReadFromFile()) {
    Log::d("Prettify file is valid");

    // [normal]
    try {
      const std::map<std::string, std::string>& norm_replace = cf.GetSection("normal");
      NormVarReplaceSet.clear();
      for (auto&& set : norm_replace) {
        NormVarReplaceSet.emplace(set.first, set.second);
      }
    } catch (std::runtime_error& rt_ex) {
      Log::w("Section \"normal\" not found. Will not replace normal fields");

      // no need to handle it
    }

    // [bool]
    try {
      const std::map<std::string, std::string>& bool_replace = cf.GetSection("bool");
      BoolVarReplaceSet.clear();
      for (auto&& set : bool_replace) {
        BoolVarReplaceSet.emplace(set.first, set.second);
      }
    } catch (std::runtime_error& rt_ex) {
      Log::w("Section \"bool\" not found. Will not replace boolean fields");

      // no need to handle it
    }

    // [lotus]
    try {
      const std::map<std::string, std::string>& lotus_replace = cf.GetSection("lotus");
      LotusVarReplaceSet.clear();
      for (auto&& set : lotus_replace) {
        LotusVarReplaceSet.emplace(set.first, set.second);
      }
    } catch (std::runtime_error& rt_ex) {
      Log::w("Section \"lotus\" not found. Will not replace item fields");

      // no need to handle it
    }

  } else {
    Log::d("Prettify file is invalid. Will not replace fields.");
  }
}

void PrettifyLine(std::string& s) {
  // do replacement for syntactical pairs
  for (const auto& p_replace : kSyntaxReplaceSet) {
    auto cmp = s.find(p_replace.first);
    if (cmp != std::string::npos) {
      s.replace(cmp, p_replace.first.length(), p_replace.second);
    }
  }

  // do replacement for normal variables
  for (const auto& p_replace : NormVarReplaceSet) {
    auto cmp = s.find(p_replace.first);
    if (cmp != std::string::npos) {
      s.replace(cmp, p_replace.first.length(), p_replace.second);
    }
  }

  // do replacement for boolean variables
  for (const auto& p_replace : BoolVarReplaceSet) {
    auto cmp = s.find(p_replace.first);
    if (cmp != std::string::npos) {
      s.replace(cmp, p_replace.first.length(), p_replace.second);

      // also replace the boolean values
      for (const auto& p_bool : kBoolReplaceSet) {
        auto val = s.find(p_bool.first);
        if (val != std::string::npos) {
          s.replace(val, p_bool.first.size(), p_bool.second);
          break;
        }
      }
    }
  }

  // do replacement for path variables
  for (const auto& p_replace : LotusVarReplaceSet) {
    auto cmp = s.find(p_replace.first);
    if (cmp != std::string::npos) {
      s.replace(cmp, p_replace.first.length(), p_replace.second);
    }
  }
}
