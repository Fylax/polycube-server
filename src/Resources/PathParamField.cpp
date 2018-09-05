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
#include <string>
#include <memory>
#include <vector>

#include "../../include/Resources/Field.h"
#include "../../include/Resources/PathParamField.h"

PathParamField::PathParamField(const std::string& name,
                               const std::vector<
                                   std::shared_ptr<Validator>
                               >& validators)
    : Field<Pistache::Rest::Request>(validators), name_(name)  {}

const std::string PathParamField::Name() const {
  return name_;
}

ErrorTag PathParamField::Validate(const Pistache::Rest::Request& value) const {
  if (!value.hasParam(name_)) return kMissingElement;

  auto parsed = value.param(name_).as<std::string>();
  for (const auto& validator : validators_) {
    if (!validator->Validate(parsed)) return ErrorTag::kBadElement;
  }
  return ErrorTag::kOk;
}
