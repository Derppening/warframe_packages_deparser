// Copyright (c) 2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for JSON-parsing functions of Packages class.
//

#include "packages.h"

#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;

namespace {
enum struct Type {
  kUnparseable,
  kArray,
  kAnonArray,
  kEmptyArray,
  kObject,
  kAnonObject,
  kEmptyObject,
  kPair,
  kEmptyPair
};

/**
 * @brief Output a package with C++ scope resolution operator format.
 *
 * @param t Type of the JSON element
 * @param full_path Full path of the JSON element
 */
void OutputScope(Type t, const std::string& full_path) {
  switch (t) {
    case Type::kUnparseable:
      cout << "UNPARSEABLE : " << full_path << "UNPARSEABLEcONTENTS" << endl;
      break;
    case Type::kEmptyArray:
      cout << "EMPTY_ARRAY : " << full_path << "[]" << endl;
      break;
    case Type::kEmptyObject:
      cout << "EMPTY_OBJECT: " << full_path << "{}" << endl;
      break;
    case Type::kAnonArray:
      cout << "ANON_ARRAY  : " << full_path << "[]" << endl;
      break;
    case Type::kArray:
      cout << "ARRAY       : " << full_path << "[]" << endl;
      break;
    case Type::kAnonObject:
      cout << "ANON_OBJECT : " << full_path << "{}" << endl;
      break;
    case Type::kObject:
      cout << "OBJECT      : " << full_path << "{}" << endl;
      break;
    case Type::kEmptyPair:
      cout << "EMPTY_PAIR  : " << full_path << endl;
      break;
    case Type::kPair:
      cout << "PAIR        : " << full_path << endl;
      break;
  }
}

/**
 * @brief Output a package with a tree format.
 *
 * @param t Type of the JSON element
 * @param full_path Full path of the JSON element
 */
void OutputTree(Type t, const unsigned indent, const std::string& key) {
  std::string str_indent = std::string((indent - 2) * 2, ' ');
  if (indent > 2) {
    for (std::string::size_type i = 0; (i = str_indent.find("    ", i)) != std::string::npos; i += 5) {
      str_indent.replace(i, 4, "  |  ");
    }
    str_indent.replace(str_indent.size() - 2, 2, "--");
  }
  std::string str;

  switch (t) {
    case Type::kEmptyArray:
      str = "[   ]";
      break;
    case Type::kArray:
    case Type::kAnonArray:
      str = "[...]";
      break;
    case Type::kEmptyObject:
      str = "{   }";
      break;
    case Type::kObject:
    case Type::kAnonObject:
      str = "{...}";
      break;
    case Type::kEmptyPair:
    case Type::kPair:
    case Type::kUnparseable:
      str = "     ";
  }

  cout << str_indent << key << str << endl;
}
}  // namespace

/**
 * @brief Convert a header into JSON format.
 *
 * @param header Header to dump
 * @param opts How to output the structure
 * @param read_file If set, read from this vector instead
 *
 * @return JSON-formatted header
 */
std::vector<std::string> Packages::HeaderToJson(const std::string& header,
                                                StructureOptions opts,
                                                std::vector<std::string>&& read_file) {
  if (headers_.find(header) == headers_.end()) {
    cout << "Cannot find header." << endl;
    return std::vector<std::string>();
  }

  std::vector<std::string> lines;
  if (read_file.empty()) {
    lines = GetHeaderContents(header, true);
  } else {
    lines = std::move(read_file);
  }
  std::vector<std::pair<char, std::string>> st;
  int indent = 0;

  std::vector<std::string> parsed;
  parsed.emplace_back("{");
  indent += 2;

  for (auto it = lines.begin(); it != lines.end(); ++it) {
    std::string& line = *it;

    while (it->front() == ' ') { line.erase(0, 1); }

    std::string::size_type entry_token = line.find('=');
    std::string::size_type barray_token = line.find("=[");
    std::string::size_type bobject_token = line.find("={");
    std::string::size_type narray_token = line.find("[]");
    std::string::size_type nobject_token = line.find("{}");
    bool aobject_token = line == "{";
    bool aarray_token = line == "[";
    bool earray_token = line == "]" || line == "],";
    bool eobject_token = line == "}" || line == "},";

    std::string st_string;
    for (const auto& s : st) {
      st_string += (s.second + "::");
    }

    if (line.find("UNPARSEABLEcONTENTS") != std::string::npos) {
      switch (opts) {
        case StructureOptions::kScope:
          OutputScope(Type::kUnparseable, st_string);
          break;
        case StructureOptions::kTree:
          OutputTree(Type::kUnparseable, unsigned(indent), "UNPARSEABLEcONTENTS");
          break;
        case StructureOptions::kNone:
          // not handled
          break;
      }
      continue;
    }

    if (narray_token != std::string::npos) {
      std::string key = line.substr(0, barray_token);

      switch (opts) {
        case StructureOptions::kScope:
          OutputScope(Type::kEmptyArray, st_string.append(key));
          break;
        case StructureOptions::kTree:
          OutputTree(Type::kEmptyArray, unsigned(indent), key);
          break;
        case StructureOptions::kNone:
          // not handled
          break;
      }

      parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(key).append(R"(":[],)"));
      continue;
    }

    if (nobject_token != std::string::npos) {
      std::string key = line.substr(0, bobject_token);

      switch (opts) {
        case StructureOptions::kScope:
          OutputScope(Type::kEmptyObject, st_string.append(key));
          break;
        case StructureOptions::kTree:
          OutputTree(Type::kEmptyObject, unsigned(indent), key);
          break;
        case StructureOptions::kNone:
          // not handled
          break;
      }

      parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(key).append(R"(":{},)"));
      continue;
    }

    if (earray_token) {
      if (st.back().first != '[') {
        std::cerr << "Expecting array, found object." << endl;
        std::cerr << "Stack Trace: " << st_string << endl;
      } else {
        st.pop_back();

        indent -= 2;
        parsed.emplace_back(std::string(unsigned(indent), ' ').append("]"));
      }
    } else if (barray_token != std::string::npos) {
      std::string key = line.substr(0, barray_token);

      switch (opts) {
        case StructureOptions::kScope:
          OutputScope(Type::kArray, st_string.append(key));
          break;
        case StructureOptions::kTree:
          OutputTree(Type::kArray, unsigned(indent), key);
          break;
        case StructureOptions::kNone:
          // not handled
          break;
      }

      st.emplace_back('[', line.substr(0, barray_token) + "[]");
      parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(key).append(R"(":[)"));
      indent += 2;

      continue;
    } else if (eobject_token) {
      if (st.back().first != '{') {
        std::cerr << "Expecting object, found array." << endl;
        std::cerr << "Stack Trace: " << st_string << endl;
      } else {
        st.pop_back();

        indent -= 2;
        parsed.emplace_back(std::string(unsigned(indent), ' ').append("}"));
      }
    } else if (aobject_token) {
      switch (opts) {
        case StructureOptions::kScope:
          OutputScope(Type::kAnonObject, st_string);
          break;
        case StructureOptions::kTree:
          OutputTree(Type::kAnonObject, unsigned(indent), "");
          break;
        case StructureOptions::kNone:
          // not handled
          break;
      }

      st.emplace_back('{', "{}");
      parsed.emplace_back(std::string(unsigned(indent), ' ').append("{"));
      indent += 2;

      continue;
    } else if (aarray_token) {
      switch (opts) {
        case StructureOptions::kScope:
          OutputScope(Type::kAnonObject, st_string);
          break;
        case StructureOptions::kTree:
          OutputTree(Type::kAnonObject, unsigned(indent), "");
          break;
        case StructureOptions::kNone:
          // not handled
          break;
      }

      st.emplace_back('[', "[]");
      parsed.emplace_back(std::string(unsigned(indent), ' ').append("["));
      indent += 2;

      continue;
    } else if (bobject_token != std::string::npos) {
      std::string key = line.substr(0, bobject_token);

      switch (opts) {
        case StructureOptions::kScope:
          OutputScope(Type::kObject, st_string.append(key));
          break;
        case StructureOptions::kTree:
          OutputTree(Type::kObject, unsigned(indent), key);
          break;
        case StructureOptions::kNone:
          // not handled
          break;
      }

      st.emplace_back('{', line.substr(0, bobject_token).append("{}"));
      parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(key).append(R"(":{)"));
      indent += 2;

      continue;
    } else if (entry_token != std::string::npos) {
      std::string key = line.substr(0, entry_token);

      if (line.substr(entry_token + 1) == "\"\"") {
        switch (opts) {
          case StructureOptions::kScope:
            OutputScope(Type::kEmptyPair, st_string.append(key));
            break;
          case StructureOptions::kTree:
            OutputTree(Type::kEmptyPair, unsigned(indent), key);
            break;
          case StructureOptions::kNone:
            // not handled
            break;
        }

        parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(key).append(R"(":"")"));
      } else {
        std::string value = line.substr(entry_token + 1);

        switch (opts) {
          case StructureOptions::kScope:
            OutputScope(Type::kPair, st_string.append(key));
            break;
          case StructureOptions::kTree:
            OutputTree(Type::kPair, unsigned(indent), key);
            break;
          case StructureOptions::kNone:
            // not handled
            break;
        }

        parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(key).append(R"(":")").append(value).append("\""));
      }
    } else {
      if (line.back() == ',') { line.pop_back(); }
      parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(line).append("\""));
    }

    if ((it + 1) != lines.end() &&
        ((it + 1)->find("[]") != std::string::npos || (it + 1)->find("{}") != std::string::npos ||
            ((it + 1)->find(']') == std::string::npos && (it + 1)->find('}') == std::string::npos))) {
      parsed.back().push_back(',');
    }
  }

  indent -= 2;
  parsed.emplace_back("}");

  if (!st.empty()) {
    std::cerr << header << ": Unable to resolve all entries" << endl;
    parsed.clear();
  } else if (indent != 0) {
    std::cerr << header << ": Unable to correctly indent all entries" << endl;
    parsed.clear();
  }

  return parsed;
}
