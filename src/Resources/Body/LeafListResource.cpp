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
#include "../../../include/Resources/Body/LeafListResource.h"

#include <Body/LeafListResource.h>
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace polycube::polycubed::Rest::Resources::Body {

LeafListResource::LeafListResource(std::vector<std::string> &&default_value)
    : LeafResource("", "", nullptr, nullptr, false, false, nullptr),
      default_(std::move(default_value)) {}

LeafListResource::LeafListResource(std::string name, std::string module,
                                   std::shared_ptr<ParentResource> parent,
                                   std::unique_ptr<JsonBodyField> &&field,
                                   bool configurable, bool mandatory,
                                   std::vector<std::string> &&default_value)
    : LeafResource(std::move(name), std::move(module), std::move(parent),
                   std::move(field), configurable, mandatory, nullptr),
      default_(std::move(default_value)) {}

std::vector<Response> LeafListResource::BodyValidate(
    nlohmann::json &body, bool check_mandatory) const {
  std::vector<Response> errors;
  if (body.type() != nlohmann::detail::value_t::array) {
    errors.push_back({ErrorTag::kBadAttribute, name_.data()});
    return errors;
  }

  for (auto &element : body.items()) {
    const auto &inner_errors =
        LeafResource::BodyValidate(element.value(), check_mandatory);
    errors.reserve(errors.size() + inner_errors.size());
    std::copy(std::begin(inner_errors), std::end(inner_errors),
              std::back_inserter(errors));
  }
  return errors;
}

void LeafListResource::SetDefaultIfMissing(nlohmann::json &body) const {
  if (body.empty() && !default_.empty()) {
    for (const auto &element : default_) {
      body.push_back(element);
    }
  }
}
}  // namespace polycube::polycubed::Rest::Resources::Body
