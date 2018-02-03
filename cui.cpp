// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for Cui class
//

#include "cui.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

Cui::Cui(HintLevel level) : hint_level_(level)
{}

void Cui::AddDiv() {
  AddHeader("");
}

void Cui::AddHeader(const std::string header) {
  AddItem(header, std::vector<std::string>());
}

void Cui::AddItem(const std::string text, const std::string input, std::function<void(std::string)> fptr, bool is_term) {
  AddItem(text, std::vector<std::string>({input}), std::move(fptr), is_term);
}

void Cui::AddItem(const std::string text, const std::vector<std::string>& input, std::function<void(std::string)> fptr, bool is_term) {
  Entry e;
  e.text = text;
  e.input = input;
  e.fptr = std::move(fptr);
  e.is_terminate = is_term;
  sel_.push_back(e);
}

bool Cui::Inflate(bool has_custom_text, bool terminate_anyways) {
  while (true) {
    if (!has_custom_text && static_cast<bool>(system("cls"))) {
      system("clear");
    }

    if (hint_level_ != HintLevel::kNone) {
      for (auto it = sel_.begin(); it != sel_.end(); ++it) {
        if (it->text.empty()) {
          std::cout << '\n';
        } else if (it->input.empty() && it == sel_.begin()) {
          std::cout << it->text << '\n';
        } else if (it->input.empty()) {
          std::cout << '\n' << it->text << '\n';
        } else {
          if (hint_level_ == HintLevel::kSimple) {
            std::cout << it->input.at(0) << ": " << it->text << '\n';
          } else {
            std::string hint_inputs;
            for (const auto& str : it->input) {
              hint_inputs += (str + ", ");
            }
            hint_inputs = hint_inputs.substr(0, hint_inputs.length() - 2);
            std::cout << hint_inputs << ": " << it->text << '\n';
          }
        }
      }
      std::cout << std::endl;
    }

    std::cout << "> ";
    std::string input;
    std::getline(std::cin, input);

    switch (Parse(input)) {
      case ParseResult::kExit:
        return true;
      case ParseResult::kCmdNotFound:
        std::cout << input << ": Not found" << std::endl;
        break;
      case ParseResult::kContinue:
      default:
        // not handled
        break;
    }

    std::cout << std::endl;
    std::cout << "Press [ENTER] to continue..." << std::endl;
    std::getline(std::cin, input);

    if (terminate_anyways) {
      return false;
    }
  }
}

Cui::ParseResult Cui::Parse(std::string arg) {
  std::string args;
  if (arg.find(' ') != std::string::npos) {
    args = arg.substr(arg.find(' ') + 1);
    arg = arg.substr(0, arg.find(' '));
  }

  auto search_res =
      std::find_if(sel_.begin(),
                   sel_.end(),
                   [&arg](const auto& entry) -> bool {
                     for (const auto& it : entry.input) {
                       if (it == arg) { return true; }
                     }
                     return false;
                   });

  if (search_res != sel_.end()) {
    if (search_res->fptr) {
      search_res->fptr(args);
    }
    if (search_res->is_terminate) {
      return ParseResult::kExit;
    }
  } else {
    return ParseResult::kCmdNotFound;
  }

  return ParseResult::kContinue;
}
