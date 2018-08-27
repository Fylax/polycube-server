#include "PatternValidator.h"
#include <string>

PatternValidator::PatternValidator(const char* pattern):
    pattern_(pattern, std::regex_constants::optimize |
                      std::regex_constants::ECMAScript) {}

bool PatternValidator::validate(const std::string& value) const {
  return std::regex_match(value, pattern_);
}
