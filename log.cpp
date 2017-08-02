// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for StaticLog class.
//

#include "log.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

using std::cerr;
using std::clog;
using std::string;

bool Log::is_init_ = false;
bool Log::enable_logging_ = false;
bool Log::use_override_pipe_ = false;
Log::Pipe Log::override_pipe_ = Log::Pipe::kDefault;

std::unique_ptr<std::string> Log::verbose_app_ = nullptr;
std::unique_ptr<std::string> Log::debug_app_ = nullptr;
std::unique_ptr<std::string> Log::info_app_ = nullptr;
std::unique_ptr<std::string> Log::warn_app_ = nullptr;
std::unique_ptr<std::string> Log::error_app_ = nullptr;

std::unique_ptr<std::size_t> Log::padding_len_ = nullptr;

std::unique_ptr<std::ofstream> Log::log_str_ = nullptr;

namespace {
void PutTime(std::stringstream& ss) {
  auto time = std::time(nullptr);
  std::tm tm = *std::localtime(&time);

  ss << std::put_time(&tm, "%m-%d %H:%M:%S");
}
}  // namespace

void Log::Init() {
  verbose_app_ = std::make_unique<std::string>("[VERBOSE]");
  debug_app_ = std::make_unique<std::string>("[DEBUG]");
  info_app_ = std::make_unique<std::string>("[INFO]");
  warn_app_ = std::make_unique<std::string>("[WARNING]");
  error_app_ = std::make_unique<std::string>("[ERROR]");

  padding_len_ = std::make_unique<std::size_t>(0);

  EvaluatePadding();

  is_init_ = true;
}

void Log::Enable() {
  if (!is_init_) {
    throw std::runtime_error("Logging class has not been initialized yet");
  }

  enable_logging_ = true;
}

void Log::Disable() {
  if (!is_init_) {
    throw std::runtime_error("Logging class has not been initialized yet");
  }

  enable_logging_ = false;
}

bool Log::SetFile(string filename, std::ios_base::openmode mode) {
  if (!enable_logging_) return false;

  if (log_str_ != nullptr) {
    log_str_->close();
  }

  log_str_ = std::make_unique<std::ofstream>(filename, mode);
  return static_cast<bool>(*log_str_);
}

bool Log::ForceSetFile(string filename, std::ios_base::openmode mode) {
  if (!enable_logging_) return false;

  if (log_str_ != nullptr) {
    log_str_->close();
  }

  log_str_ = std::make_unique<std::ofstream>(filename, mode);
  if (!*log_str_) {
    throw std::ios_base::failure("Unable to open file for write");
  }

  return true;
}

void Log::v(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss <<  '\t' << *verbose_app_ << GetPadding(kVerbose) << ": " << message << '\n';

  if (use_override_pipe_) {
    dest = override_pipe_;
  }

  switch (dest) {
    default:
    case Pipe::kDefault:
    case Pipe::kClog:
      clog << ss.str();
      break;
    case Pipe::kFile: {
      if (!*log_str_) {
        throw std::runtime_error("No file open for logging");
      }
      *log_str_ << ss.str();
    }
  }
}

void Log::d(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss << '\t' << *debug_app_ << GetPadding(kDebug) << ": " << message << '\n';

  if (use_override_pipe_) {
    dest = override_pipe_;
  }

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
      *log_str_ << ss.str();
    }
  }
}

void Log::i(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss << '\t' << *info_app_ << GetPadding(kInfo) << ": " << message << '\n';

  if (use_override_pipe_) {
    dest = override_pipe_;
  }

  switch (dest) {
    default:
    case Pipe::kDefault:
    case Pipe::kClog:
      clog << ss.str();
      break;
    case Pipe::kFile: {
      if (!*log_str_) {
        throw std::runtime_error("No file open for logging");
      }
      *log_str_ << ss.str();
    }
  }
}

void Log::w(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss << '\t' << *warn_app_ << GetPadding(kWarn) << ": " << message << '\n';

  if (use_override_pipe_) {
    dest = override_pipe_;
  }

  switch (dest) {
    default:
    case Pipe::kDefault:
    case Pipe::kClog:
      clog << ss.str();
      break;
    case Pipe::kFile: {
      if (!*log_str_) {
        throw std::runtime_error("No file open for logging");
      }
      *log_str_ << ss.str();
    }
  }
}

void Log::e(string message, Pipe dest) {
  if (!enable_logging_) return;

  std::stringstream ss;
  PutTime(ss);
  ss << '\t' << *error_app_ << GetPadding(kError) << ": " << message << '\n';

  if (use_override_pipe_) {
    dest = override_pipe_;
  }

  switch (dest) {
    default:
    case Pipe::kDefault:
      cerr << ss.str();
      break;
    case Pipe::kClog:
      clog << ss.str();
      break;
    case Pipe::kFile: {
      if (!*log_str_) {
        throw std::runtime_error("No file open for logging");
      }
      *log_str_ << ss.str();
    }
  }
}

void Log::FlushStderrBuf() {
  clog.flush();
}

void Log::FlushFileBuf() {
  if (log_str_ != nullptr) {
    log_str_->flush();
  }
}

void Log::SetVerboseString(string str) {
  *verbose_app_ = std::move(str);
  EvaluatePadding();
}

void Log::SetDebugString(string str) {
  *debug_app_ = std::move(str);
  EvaluatePadding();
}

void Log::SetInfoString(string str) {
  *info_app_ = std::move(str);
  EvaluatePadding();
}

void Log::SetWarningString(string str) {
  *warn_app_ = std::move(str);
  EvaluatePadding();
}

void Log::SetErrorString(string str) {
  *error_app_ = std::move(str);
  EvaluatePadding();
}

void Log::EvaluatePadding() {
  *padding_len_ = verbose_app_->length();
  *padding_len_ = std::max(*padding_len_, debug_app_->length());
  *padding_len_ = std::max(*padding_len_, info_app_->length());
  *padding_len_ = std::max(*padding_len_, warn_app_->length());
  *padding_len_ = std::max(*padding_len_, error_app_->length());
}

auto Log::GetPadding(Level lvl) -> std::string {
  std::size_t i = *padding_len_;

  switch (lvl) {
    case kVerbose:
      i -= verbose_app_->length();
      break;
    case kDebug:
      i -= debug_app_->length();
      break;
    case kInfo:
      i -= info_app_->length();
      break;
    case kWarn:
      i -= warn_app_->length();
      break;
    case kError:
      i -= error_app_->length();
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
