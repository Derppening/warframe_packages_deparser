// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities for exporting and importing fixed-format files.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_CONFIG_FILE_H_
#define WARFRAME_PACKAGES_DEPARSER_CONFIG_FILE_H_

#include <map>
#include <memory>
#include <string>

/**
 * Class to manage application configurations.
 *
 * Configuration can be used within the application, as well as export/import to a savefile.
 */
class ConfigFile {
 public:
  /**
   * Default constructor. Used only for local storage, unless filename is set afterwards.
   */
  ConfigFile();
  /**
   * Default constructor with filename
   *
   * @param filename Filename of configuration to read from/write to
   */
  explicit ConfigFile(std::string filename);
  /**
   * Move constructor.
   *
   * @param config Existing ConfigFile class
   */
  ConfigFile(ConfigFile&& config) noexcept;
  /**
   * Copy constructor.
   *
   * @param config Existing ConfigFile class
   */
  ConfigFile(const ConfigFile& config);

  ~ConfigFile();

  /**
   * Move assignment operator.
   *
   * @return Newly created object from existing @c ConfigFile.
   */
  auto operator=(ConfigFile&&) noexcept -> ConfigFile&;
  /**
   * Copy assignment operator.
   *
   * @return Newly created object from existing @c ConfigFile.
   */
  auto operator=(const ConfigFile&) -> ConfigFile&;

  /**
   * @param filename Sets the destination file to read configuration from/write to
   */
  void SetFile(std::string filename);

  /**
   * Get the configuration data from a given field
   *
   * @param section Section Name
   * @param field Field Name
   * @return Data from the given field
   *
   * @throw @c std::range_error if the section or field is not found.
   */
  auto GetField(std::string section, std::string field) -> std::string;
  /**
   * Write the configuration to a given field
   *
   * @param section Section Name
   * @param field Field Name
   * @param data Information to write
   */
  void WriteField(std::string section, std::string field, std::string data);
  /**
   * Erase a field from the configuration storage
   *
   * @param section
   * @param field
   */
  void EraseField(std::string section, std::string field);

  /**
   * Retrieves a reference to the map of a given section
   *
   * @param section Name of section to get
   * @return Map of section
   *
   * @throw @c std::runtime_error if the section is not found
   */
  auto GetSection(std::string section) -> const std::map<std::string, std::string>&;
  /**
   * Retrieves a reference to the whole config
   *
   * @return Reference to config
   */
  auto GetConfig() -> const std::map<std::string, std::map<std::string, std::string>>&;

  /**
   * Export the current configuration to a file.
   *
   * @note This will call @c GarbageCollect() to remove unused sections.
   *
   * @return True if successful.
   */
  bool ExportToFile();
  /**
   * Read a configuration from a file. The current stored configuration will be erased.
   *
   * @param dispose_sectionless Dispose all fields which does not have a section associated.
   *
   * @return True if successful.
   */
  bool ReadFromFile(bool dispose_sectionless = true);

  /**
   * Remove all fields and sections from the current configuration.
   */
  void Clear();
  /**
   * Removes all sections which do not have any fields associates with.
   */
  void GarbageCollect();

 private:
  std::string filename_ = "";
  std::unique_ptr<std::map<std::string, std::map<std::string, std::string>>> config_ = nullptr;
};

#endif //WARFRAME_PACKAGES_DEPARSER_CONFIG_FILE_H_
