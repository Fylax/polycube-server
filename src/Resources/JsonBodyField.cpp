#include "../../include/Reources/JsonBodyField.h"

#include <string>
#include <memory>
#include <vector>

#include "../../externals/include/nlohmann/json.hpp"

JsonBodyField::JsonBodyField(std::string& name,
                             std::vector<std::shared_ptr<Validator>>& validators)
    : Field(name, validators) {}

bool JsonBodyField::validate(Pistache::Rest::Request request) const {
  auto json = nlohmann::json::parse(request.body());

  bool exists = json.count(name_) == 1;
  if (!exists) return false;

  auto value = json.at(name_).get<std::string>();

  for (const auto& validator: validators_) {
    if (validator->validate(value)) return false;
  }
  return true;
}
