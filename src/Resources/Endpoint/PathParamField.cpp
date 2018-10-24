/*
 * Copyright 2018 The Polycube Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../../../include/Resources/Endpoint/PathParamField.h"

#include <memory>
#include <vector>

#include "../../../include/Validators/EmptyValidator.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
PathParamField::PathParamField(
    const std::string &name,
    std::vector<std::shared_ptr<Validators::Validator>> validators)
    : Field<Pistache::Rest::Request>(std::move(validators)),
      name_(std::string{':'} + name) {
  for (const auto &validator : validators_) {
    if (auto test =
            std::dynamic_pointer_cast<Validators::EmptyValidator>(validator)) {
      test->IsPath(true);
    }
  }
}

const std::string &PathParamField::Name() const {
  return name_;
}

ErrorTag PathParamField::Validate(const Pistache::Rest::Request &value) const {
  if (!value.hasParam(name_))
    return kMissingElement;
  if (!Validate(value.param(name_).as<std::string>()))
    return ErrorTag::kBadElement;
  return ErrorTag::kOk;
}

bool PathParamField::Validate(const std::string &value) const {
  for (const auto &validator : validators_) {
    if (!validator->Validate(value))
      return false;
  }
  return true;
}
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
