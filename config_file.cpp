// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for ConfigFile class
//

#include "config_file.h"

#include <algorithm>
#include <fstream>
#include <memory>

ConfigFile::ConfigFile() {
  config_ = std::make_unique<std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>>();
}

ConfigFile::ConfigFile(std::string filename) : filename_(std::move(filename)) {
  config_ = std::make_unique<std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>>();
}

ConfigFile::ConfigFile(ConfigFile&& config) noexcept
    : filename_(std::move(config.filename_)), config_(std::move(config.config_))
{}

ConfigFile::ConfigFile(const ConfigFile& config) : filename_(config.filename_) {
  config_ = std::make_unique<std::vector<std::pair<std::string,
                                                   std::vector<std::pair<std::string, std::string>>>>>(*config.config_);
}

ConfigFile::~ConfigFile() {
  config_.reset();
}

auto ConfigFile::operator=(ConfigFile&& config) noexcept -> ConfigFile& {
  filename_ = std::move(config.filename_);
  config_ = std::move(config.config_);

  return *this;
}

auto ConfigFile::operator=(const ConfigFile& config) -> ConfigFile& {
  filename_ = config.filename_;
  config_ = std::make_unique<std::vector<std::pair<std::string,
                                                         std::vector<std::pair<std::string,
                                                                               std::string>>>>>(*config.config_);
  return *this;
}

void ConfigFile::SetFile(std::string filename) {
  filename_ = std::move(filename);
}

auto ConfigFile::GetField(std::string section, std::string field) -> std::string {
  // find the section
  const auto config_it = std::find_if(config_->begin(),
                                      config_->end(),
                                      [&section](const auto& p) -> bool { return p.first == section; });
  if (config_it == config_->end()) {
    throw std::range_error("Cannot find section " + section);
  }

  // use the section and find the field
  const auto& section_ptr = config_it->second;
  const auto field_it = std::find_if(section_ptr.begin(),
                                     section_ptr.end(),
                                     [&field](const auto& p) -> bool { return p.first == field; });
  if (field_it == section_ptr.end()) {
    throw std::range_error("Cannot find field: " + section + "::" + field);
  }
  return field_it->second;
}

void ConfigFile::WriteField(std::string section, std::string field, std::string data) {
  // find the section
  auto config_it = std::find_if(config_->begin(),
                                config_->end(),
                                [&section](const auto& p) -> bool { return p.first == section; });
  if (config_it == config_->end()) {
    config_->push_back({section, std::vector<std::pair<std::string, std::string>>()});
    config_it = config_->end() - 1;
  }

  // find the field
  auto& section_ref = config_it->second;
  auto field_it = std::find_if(section_ref.begin(),
                               section_ref.end(),
                               [&field](const auto& p) -> bool { return p.first == field; });

  // create the field if it's not found
  if (field_it == section_ref.end()) {
    section_ref.emplace_back(field, data);
  } else {
    field_it->second = data;
  }
}

void ConfigFile::EraseField(std::string section, std::string field) {
  // find the section
  auto config_it = std::find_if(config_->begin(),
                                config_->end(),
                                [&section](const auto& p) -> bool { return p.first == section; });
  if (config_it == config_->end()) {
    return;
  }

  // find the field
  auto& section_ref = config_it->second;
  auto field_it = std::find_if(section_ref.begin(),
                               section_ref.end(),
                               [&field](const auto& p) -> bool { return p.first == field; });

  // erase the field if it's found
  if (field_it != section_ref.end()) {
    section_ref.erase(field_it);
  }
}

bool ConfigFile::ExportToFile() {
  // remove redundant sections from the config vector
  GarbageCollect();

  // sort the config vector by alphabetical order
  std::sort(config_->begin(), config_->end(), [](const auto& a, const auto& b) -> bool { return a.first < b.first; });

  // open file for write
  auto file_ostr = std::ofstream(filename_);
  if (!file_ostr) {
    return false;
  }

  // loop through the section headers
  for (auto&& section : *config_) {
    // dump section header
    if (section.first.length() != 0) {
      file_ostr << "[" << section.first << "]" << '\n';
    }

    // sort section contents
    std::sort(section.second.begin(),
              section.second.end(),
              [](const auto& a, const auto& b) -> bool { return a.first < b.first; });

    // dump config contents to file
    for (const auto& field : section.second) {
      file_ostr << field.first << "=" << field.second << '\n';
    }
    file_ostr << std::endl;
  }

  return true;
}

bool ConfigFile::ReadFromFile(bool dispose_sectionless) {
  Clear();

  // open the config file
  auto file_istr = std::ifstream(filename_);
  if (!file_istr) {
    return false;
  }

  // temporary variables
  std::string line;
  std::string section;

  // loop through all lines
  while (std::getline(file_istr, line)) {
    // get the position of '='
    auto equal_pos = line.find('=');

    if (line.length() == 0) {
      // empty line
      continue;
    }
    if (line.front() == '[' && line.back() == ']') {
      // section
      section = line.substr(1, line.length() - 2);
      continue;
    }
    if (equal_pos == std::string::npos) {
      // line does not have format. not parsing
      continue;
    }

    if (section.empty() && !dispose_sectionless) {
      // force parsing lines without a section header
      WriteField("", line.substr(0, equal_pos), line.substr(equal_pos + 1));
    } else if (!section.empty()) {
      // parse lines with a section header
      WriteField(section, line.substr(0, equal_pos), line.substr(equal_pos + 1));
    }
  }

  return true;
}

void ConfigFile::GarbageCollect() {
  // loop through all sections
  for (auto&& section : *config_) {
    // if a section is empty, remove it from the vector
    if (section.second.empty()) {
      config_->erase(std::find_if(config_->begin(),
                                  config_->end(),
                                  [&section](const auto& p) -> bool { return p.first == section.first; }));
    }
  }
}

void ConfigFile::Clear() {
  config_->clear();
}
