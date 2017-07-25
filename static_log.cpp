// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for StaticLog class.
//

#include "static_log.h"

#include <iomanip>
#include <iostream>

using std::cerr;
using std::clog;
using std::string;

bool StaticLog::enable_logging_ = false;
std::string StaticLog::verbose_app_ = "[VERBOSE]";
std::string StaticLog::debug_app_ = "[DEBUG]";
std::string StaticLog::info_app_ = "[INFO]";
std::string StaticLog::warn_app_ = "[WARNING]";
std::string StaticLog::error_app_ = "[ERROR]";
std::size_t StaticLog::padding_len_ = EvaluatePadding();
std::ofstream StaticLog::log_str_;

namespace {
auto PutTime = [](std::stringstream& ss) -> void {
  auto time = std::time(nullptr);
  std::tm tm = *std::localtime(&time);

  ss << std::put_time(&tm, "%m-%d %H:%M:%S");
};
}  // namespace

bool StaticLog::SetFile(string filename, std::ios_base::openmode mode) {
  if (!enable_logging_) return false;

  log_str_.close();

  log_str_ = std::ofstream(filename, mode);
  return static_cast<bool>(log_str_);
}

bool StaticLog::ForceSetFile(string filename, std::ios_base::openmode mode) {
  if (!enable_logging_) return false;

  log_str_.close();

  log_str_ = std::ofstream(filename, mode);
  if (!log_str_) {
    throw std::ios_base::failure("Unable to open file for write");
  }

  return true;
}

void StaticLog::v(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss <<  '\t' << verbose_app_ << GetPadding(kVerbose) << ": " << message << '\n';

  switch (dest) {
    default:
    case Pipe::kDefault:
    case Pipe::kClog:
      clog << ss.str();
      break;
    case Pipe::kFile: {
      if (!log_str_) {
        throw std::runtime_error("No file open for logging");
      }
      log_str_ << ss.str();
    }
  }
}

void StaticLog::d(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss << '\t' << debug_app_ << GetPadding(kDebug) << ": " << message << '\n';

  switch (dest) {
    default:
    case Pipe::kDefault:
    case Pipe::kClog:
      clog << ss.str();
      break;
    case Pipe::kFile: {
      if (!log_str_) {
        throw std::runtime_error("No file open for logging");
      }
      log_str_ << ss.str();
    }
  }
}

void StaticLog::i(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss << '\t' << info_app_ << GetPadding(kInfo) << ": " << message << '\n';

  switch (dest) {
    default:
    case Pipe::kDefault:
    case Pipe::kClog:
      clog << ss.str();
      break;
    case Pipe::kFile: {
      if (!log_str_) {
        throw std::runtime_error("No file open for logging");
      }
      log_str_ << ss.str();
    }
  }
}

void StaticLog::w(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss << '\t' << warn_app_ << GetPadding(kWarn) << ": " << message << '\n';

  switch (dest) {
    default:
    case Pipe::kDefault:
    case Pipe::kClog:
      clog << ss.str();
      break;
    case Pipe::kFile: {
      if (!log_str_) {
        throw std::runtime_error("No file open for logging");
      }
      log_str_ << ss.str();
    }
  }
}

void StaticLog::e(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss << '\t' << error_app_ << GetPadding(kError) << ": " << message << '\n';

  switch (dest) {
    default:
    case Pipe::kDefault:
      cerr << ss.str();
      break;
    case Pipe::kClog:
      clog << ss.str();
      break;
    case Pipe::kFile: {
      if (!log_str_) {
        throw std::runtime_error("No file open for logging");
      }
      log_str_ << ss.str();
    }
  }
}

void StaticLog::FlushStderrBuf() {
  clog.flush();
}

void StaticLog::FlushFileBuf() {
  log_str_.flush();
}

void StaticLog::SetVerboseString(string str) {
  verbose_app_ = std::move(str);
  EvaluatePadding();
}

void StaticLog::SetDebugString(string str) {
  debug_app_ = std::move(str);
  EvaluatePadding();
}

void StaticLog::SetInfoString(string str) {
  info_app_ = std::move(str);
  EvaluatePadding();
}

void StaticLog::SetWarningString(string str) {
  warn_app_ = std::move(str);
  EvaluatePadding();
}

void StaticLog::SetErrorString(string str) {
  error_app_ = std::move(str);
  EvaluatePadding();
}

std::size_t StaticLog::EvaluatePadding() {
  padding_len_ = verbose_app_.length();
  padding_len_ = std::max(padding_len_, debug_app_.length());
  padding_len_ = std::max(padding_len_, info_app_.length());
  padding_len_ = std::max(padding_len_, warn_app_.length());
  padding_len_ = std::max(padding_len_, error_app_.length());

  return padding_len_;
}

auto StaticLog::GetPadding(Level lvl) -> std::string {
  std::size_t i = padding_len_;

  switch (lvl) {
    case kVerbose:
      i -= verbose_app_.length();
      break;
    case kDebug:
      i -= debug_app_.length();
      break;
    case kInfo:
      i -= info_app_.length();
      break;
    case kWarn:
      i -= warn_app_.length();
      break;
    case kError:
      i -= error_app_.length();
      break;
    default:
      // all cases covered
      break;
  }

  std::string s;
  for (unsigned int it = 0; it < i; ++it) {
    s += " ";
  }

  return s;
}
