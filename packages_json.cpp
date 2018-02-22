#include "packages.h"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "util.h"

using std::cout;
using std::endl;

std::vector<std::string> Packages::HeaderToJson(const std::string& header) {
  if (headers_.find(header) == headers_.end()) {
    cout << "Cannot find header." << endl;
    return std::vector<std::string>();
  }

  std::vector<std::string> lines = GetHeaderContents(header, true);
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
    bool earray_token = line == "]";
    bool eobject_token = line == "}" || line == "},";

    std::string st_string;
    for (const auto& s : st) {
      st_string += (s.second + "::");
    }

    if (line.find("UNPARSEABLEcONTENTS") != std::string::npos) {
      cout << "UNPARSEABLE : " << st_string << "UNPARSEABLEcONTENTS" << endl;
      continue;
    }

    if (narray_token != std::string::npos) {
      std::string key = line.substr(0, barray_token);
      cout << "EMPTY_ARRAY : " << st_string << key << "[]" << endl;
      parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(key).append(R"(":[],)"));
      continue;
    }
    if (nobject_token != std::string::npos) {
      std::string key = line.substr(0, bobject_token);
      cout << "EMPTY_OBJECT: " << st_string << line.substr(0, barray_token) << "{}" << endl;
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

      cout << "ARRAY       : " << st_string << key << "[]" << endl;
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
      cout << "ANON_OBJECT : " << st_string << "{}" << endl;
      st.emplace_back('{', "{}");

      parsed.emplace_back(std::string(unsigned(indent), ' ').append("{"));
      indent += 2;

      continue;
    } else if (bobject_token != std::string::npos) {
      std::string key = line.substr(0, bobject_token);

      cout << "OBJECT      : " << st_string << key << "{}" << endl;
      st.emplace_back('{', line.substr(0, bobject_token).append("{}"));

      parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(key).append(R"(":{)"));
      indent += 2;

      continue;
    } else if (entry_token != std::string::npos) {
      std::string key = line.substr(0, entry_token);

      if (line.substr(entry_token + 1) == "\"\"") {
        cout << "EMPTY_PAIR  : " << st_string << key << endl;
        parsed.emplace_back(std::string(unsigned(indent), ' ').append("\"").append(key).append(R"(":"")"));
      } else {
        std::string value = line.substr(entry_token + 1);

        cout << "PAIR        : " << st_string << key << endl;

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
    std::cerr << "Unable to resolve all entries" << endl;
  } else if (indent != 0) {
    std::cerr << "Unable to correctly indent all entries" << endl;
  }

  return parsed;
}
