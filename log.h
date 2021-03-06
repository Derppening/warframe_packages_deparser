// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities for logging.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_STATIC_LOG_H_
#define WARFRAME_PACKAGES_DEPARSER_STATIC_LOG_H_

#include <fstream>
#include <memory>
#include <string>

class Log {
 public:
  /**
   * Enumeration of piping destinations.
   */
  enum Pipe {
    /**
     * Default pipe
     */
        kDefault,
    /**
     * Pipe to clog (buffered stderr)
     */
        kClog,
    /**
     * Pipe to file
     */
        kFile,
  };

  /**
   * Enumeration of error levels
   */
  enum Level {
    kVerbose,
    kDebug,
    kInfo,
    kWarn,
    kError
  };

  Log() = delete;
  Log(Log&&) = delete;
  Log(const Log&) = delete;
  auto operator=(Log&&) noexcept -> Log& = delete;
  auto operator=(const Log&) -> Log& = delete;
  ~Log() = delete;

  /**
   * Initializes all fields in this class.
   *
   * @note This function must be called before any other methods are used.
   */
  static void Init();

  /**
   * Enable all logging functions provided by this class.
   *
   * @throw @c std::runtime_error if class is not initialized.
   */
  static void Enable();
  /**
   * Disable all logging functions provided by this class.
   *
   * @throw @c std::runtime_error if class is not initialized.
   */
  static void Disable();

  /**
   * Sets the output file for logging.
   *
   * @param filename Name of new output file
   * @param mode Mode to output to
   *
   * @return True if successful
   */
  static bool SetFile(std::string filename, std::ios_base::openmode mode = std::ios_base::out);
  /**
   * Sets the output file for logging.
   *
   * @param filename Name of new output file
   * @param mode Mode to output to
   *
   * @return True if successful
   * @throw @c std::ios_base_failure if stream is not good
   */
  static bool ForceSetFile(std::string filename, std::ios_base::openmode mode = std::ios_base::out);

  /**
   * Set whether to use the override pipe by default.
   *
   * @param use_default_pipe True if using the override pipe by default
   */
  static void UseOverridePipe(bool use_default_pipe) { use_override_pipe_ = use_default_pipe; }
  /**
   * Set which pipe to use when overriding the default pipe.
   *
   * @param default_pipe Pipe to use
   */
  static void SetOverridePipe(Pipe default_pipe) { override_pipe_ = default_pipe; }

  /**
 * Log a message with verbose severity.
 *
 * @param message Message contents
 * @param dest Destination to pipe to
 *
 * @throw @c std::runtime_error if a file is not specified when destination is set to kFile
 */
  static void v(std::string message, Pipe dest = kDefault);
  /**
   * Log a message with debug severity.
   *
   * @param message Message contents
   * @param dest Destination to pipe to
   *
   * @throw @c std::runtime_error if a file is not specified when destination is set to kFile
   */
  static void d(std::string message, Pipe dest = kDefault);
  /**
   * Log a message with info severity.
   *
   * @param message Message contents
   * @param dest Destination to pipe to
   *
   * @throw @c std::runtime_error if a file is not specified when destination is set to kFile
   */
  static void i(std::string message, Pipe dest = kDefault);
  /**
   * Log a message with warning severity.
   *
   * @param message Message contents
   * @param dest Destination to pipe to
   *
   * @throw @c std::runtime_error if a file is not specified when destination is set to kFile
   */
  static void w(std::string message, Pipe dest = kDefault);
  /**
   * Log a message with error severity.
   *
   * @param message Message contents
   * @param dest Destination to pipe to
   *
   * @throw @c std::runtime_error if a file is not specified when destination is set to kFile
   */
  static void e(std::string message, Pipe dest = kDefault);

  /**
   * Flushes the stderr buffer.
   */
  static void FlushStderrBuf();
  /**
   * Flushes the file buffer.
   */
  static void FlushFileBuf();

  /**
 * Sets the tag for verbose messages.
 *
 * @param str New tag
 */
  static void SetVerboseString(std::string str);
  /**
   * Sets the tag for debug messages.
   *
   * @param str New tag
   */
  static void SetDebugString(std::string str);
  /**
   * Sets the tag for info messages.
   *
   * @param str New tag
   */
  static void SetInfoString(std::string str);
  /**
   * Sets the tag for warning messages.
   *
   * @param str New tag
   */
  static void SetWarningString(std::string str);
  /**
   * Sets the tag for error messages.
   *
   * @param str New tag
   */
  static void SetErrorString(std::string str);

 private:
  static void EvaluatePadding();
  static auto GetPadding(Level lvl) -> std::string;

  static bool is_init_;
  static bool enable_logging_;

  static bool use_override_pipe_;
  static Pipe override_pipe_;

  static std::unique_ptr<std::string> verbose_app_;
  static std::unique_ptr<std::string> debug_app_;
  static std::unique_ptr<std::string> info_app_;
  static std::unique_ptr<std::string> warn_app_;
  static std::unique_ptr<std::string> error_app_;

  static std::unique_ptr<std::size_t> padding_len_;

  static std::unique_ptr<std::ofstream> log_str_;
};

#endif  // WARFRAME_PACKAGES_DEPARSER_STATIC_LOG_H_
