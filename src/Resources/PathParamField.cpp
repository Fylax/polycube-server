#include <string>
#include <memory>
#include <vector>

#include "../../include/Reources/PathParamField.h"

PathParamField::PathParamField(std::string& name,
                               std::vector<std::shared_ptr<Validator>>& validators)
    : Field(name, validators) {}

bool PathParamField::validate(Pistache::Rest::Request request) const {
  auto param = request.param(name_).as<std::string>();
  for (const auto& validator: validators_) {
    if (validator->validate(param)) return false;
  }
  return true;
}
