#include "../../include/Validators/EnumValidator.h"

#include <unordered_set>
#include <string>
#include <utility>

EnumValidator::EnumValidator(std::unordered_set<std::string>& enums):
    values_(std::move(enums)) {}

bool EnumValidator::validate(const std::string& value) const {
  return values_.count(value) == 1;
}
