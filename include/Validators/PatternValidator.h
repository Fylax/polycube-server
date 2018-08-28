#ifndef PARSER_PATTERNVALIDATOR_H
#define PARSER_PATTERNVALIDATOR_H

#include <regex>
#include <string>
#include "Validator.h"


class PatternValidator: public Validator {
 public:
  /**
   *
   * @param pattern
   * @throws std::regex_error
   */
  explicit PatternValidator(const char* pattern);
  bool validate(const std::string& value) const override;

 private:
  const std::regex pattern_;
};


#endif //PARSER_PATTERNVALIDATOR_H
