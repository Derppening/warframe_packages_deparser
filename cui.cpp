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

bool Cui::Inflate() {
  while (true) {
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

    if (input.empty()) { continue; }

    std::string args;
    if (input.find(' ') != std::string::npos) {
      args = input.substr(input.find(' ') + 1);
      input = input.substr(0, input.find(' '));
    }

    auto search_res =
        std::find_if(sel_.begin(),
                     sel_.end(),
                     [&input](const auto& entry) -> bool {
                       for (const auto& it : entry.input) {
                         if (it == input) { return true; }
                       }
                       return false;
                     });

    if (search_res != sel_.end()) {
      if (search_res->fptr) {
        search_res->fptr(args);
      }
      if (search_res->is_terminate) {
        return true;
      }
    } else {
      std::cout << input << ": Not found" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Press [ENTER] to continue..." << std::endl;
    std::getline(std::cin, input);

    return false;
  }
}
