// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities for a console interface.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_CUI_H_
#define WARFRAME_PACKAGES_DEPARSER_CUI_H_

#include <functional>
#include <string>
#include <vector>

class Cui {
 public:
  /**
   * Enumeration of the verboseness of the hints.
   */
  enum HintLevel {
    kNone,
    kSimple,
    kAll
  };

  enum ParseResult {
    kCmdNotFound = -1,
    kContinue,
    kExit
  };

  /**
   * Default Constructor. Defaults to hint level @c kSimple.
   */
  Cui() = default;
  /**
   * Constructor accepting @c HintLevel
   *
   * @param level The target verboseness of @c Cui.
   */
  explicit Cui(HintLevel level);

  /**
   * Adds a dividing line.
   */
  void AddDiv();
  /**
   * Adds a header line.
   *
   * @param header Header text
   */
  void AddHeader(std::string header);
  /**
   * Adds an option for user selection.
   *
   * @note Use @c std::bind to delegate the option to functions.
   * @note If the option allows the user to add additional arguments, the arguments can be accessed by
   * @c std::placeholders::_1.
   *
   * @param text Description Text
   * @param input Input text to trigger option
   * @param fptr Function object to trigger
   * @param is_term Whether to terminate @c Cui after this is run
   */
  void AddItem(std::string text,
               std::string input,
               std::function<void(std::string)> fptr = nullptr,
               bool is_term = false);
  /**
   * Adds an option for user selection.
   *
   * @note Use @c std::bind to delegate the option to functions.
   * @note If the option allows the user to add additional arguments, the arguments can be accessed by
   * @c std::placeholders::_1.
   *
   * @param text Description Text
   * @param input Input texts to trigger option
   * @param fptr Function object to trigger
   * @param is_term Whether to terminate @c Cui after this is run
   */
  void AddItem(std::string text,
               const std::vector<std::string>& input,
               std::function<void(std::string)> fptr = nullptr,
               bool is_term = false);
  /**
   * Start the @c Cui menu using the given options.
   *
   * @param has_custom_text True if displaying custom text (i.e. screen is not automatically cleared on selection)
   * @param terminate_anyways True if user input loop is handled externally
   *
   * @return True if terminating from loop
   */
  bool Inflate(bool has_custom_text = false, bool terminate_anyways = false);

  /**
   * Parses the user input and delegates to the corresponding function.
   *
   * @param arg String of user input
   * @return Result of parsing
   */
  ParseResult Parse(std::string arg);

 private:
  struct Entry {
    std::string text = "";
    std::vector<std::string> input = std::vector<std::string>();
    std::function<void(std::string)> fptr = nullptr;

    bool is_terminate = false;
  };

  HintLevel hint_level_ = kSimple;
  std::vector<Entry> sel_;
};

#endif //WARFRAME_PACKAGES_DEPARSER_CUI_H_
