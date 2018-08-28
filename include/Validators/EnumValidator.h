#ifndef PARSER_ENUMVALIDATOR_H
#define PARSER_ENUMVALIDATOR_H


#include "Validator.h"

#include <unordered_set>
#include <string>

class EnumValidator: public Validator {
 public:
  explicit EnumValidator(std::unordered_set<std::string>& enums);
  bool validate(const std::string& value) const override;

 private:
  const std::unordered_set<std::string> values_;
};


#endif //PARSER_ENUMVALIDATOR_H
