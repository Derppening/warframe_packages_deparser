// Copyright (c) 2018 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for initialization methods.
//

#include "init.h"

#include <iostream>
#include <string>

#include "log.h"

/**
 * @brief Performs initialization.
 */
void Init() {
  // initialize the logging class
  Log::Init();
  Log::Enable();
}

/**
 * @brief Outputs help text.
 *
 * @param s Name of the application
 */
void OutputHelp(const std::string& s) {
  std::string message;
  message += "Usage: " + s + " [OPTION]... -- [MODE] [MODE_ARGS]...\n";
  message += "  -D, --no-debug\t\tdisable logging\n";
  message += "  -f, --file=[FILE]\tread Packages.txt from [FILE]\n";
  message += "  -I, --no-interactive\tdisable interactive mode\n";
  message += "  -p, --prettify=[FILE]\timport prettifying replacement pairs from [FILE]\n";
  message += "      --help\t\tdisplay this help and exit\n";
  message += "      --version\t\toutput version information and exit\n\n";
  message += "MODE and MODE_ARGS will only be parsed if \'--no-interactive\' is provided.\n";
  message += "For help on using interactive mode, provide \'help\' to MODE.\n";

  std::cout << message << std::endl;
}

/**
 * @brief Outputs version information.
 */
void OutputVersionInfo() {
  std::string message;
  message += "Warframe Package Deparser " + kBuildString + "\n";
  message += "Copyright (C) 2017-2018 David Mak\n";
  message += "Licensed under MIT.";

  std::cout << message << std::endl;
}
