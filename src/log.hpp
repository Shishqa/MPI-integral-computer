#ifndef _LOG_HPP
#define _LOG_HPP

#include <iostream>
#include <fstream>

namespace compute {

class Logger {
 public:
  enum VerbosityLevel {
    LESS,
    ALL,
  };

  Logger(VerbosityLevel verbosity_level)
    : verbosity_level_(verbosity_level) {
    devnull_.open("/dev/null");
  }

  ~Logger() {
    devnull_.close();
  }

  std::ostream& Log(VerbosityLevel verbosity_level) {
    if (verbosity_level <= verbosity_level_) {
      return std::cout;
    }
    return devnull_;
  }

 private:
  VerbosityLevel verbosity_level_;
  std::ofstream devnull_;
};

}  // namespace compute

#endif  // _LOG_HPP
