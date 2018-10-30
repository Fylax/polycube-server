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

#include "../../../include/Resources/Body/LeafResource.h"
#include "../../../include/Resources/Body/JsonBodyField.h"

#include "../../../include/Server/RestServer.h"

namespace polycube::polycubed::Rest::Resources::Body {
using Pistache::Http::ResponseWriter;
using Pistache::Rest::Request;

LeafResource::LeafResource(std::string name, std::string module,
                           std::shared_ptr<ParentResource> parent,
                           std::unique_ptr<JsonBodyField> &&field,
                           bool configuration, bool mandatory,
                           std::unique_ptr<const std::string> &&default_value)
    : Resource(std::move(name), std::move(module), std::move(parent),
               configuration),
      field_(std::move(field)),
      mandatory_(mandatory),
      default_(std::move(default_value)) {}

std::vector<Response> LeafResource::BodyValidate(
    nlohmann::json &body, [[maybe_unused]] bool check_mandatory) const {
  std::vector<Response> errors;
  if (body.empty()) {
    errors.push_back({ErrorTag::kMissingAttribute, name_.data()});
    return errors;
  }

  auto field = field_->Validate(body);
  if (field != kOk) {
    errors.push_back({field, name_.data()});
  }
  return errors;
}

bool LeafResource::IsMandatory() const {
  return mandatory_;
}

bool LeafResource::IsConfiguration() const {
  return configuration_;
}

void LeafResource::SetDefaultIfMissing(nlohmann::json &body) const {
  if (body.empty() && default_ != nullptr)
    body = *default_;
}

bool LeafResource::ValidateXPath(const std::string &xpath) const {
  auto ns_pos = xpath.find(':');  // current namespace delimiter
  std::string name;
  if (ns_pos != std::string::npos) {  // fully-qualified name
    name = xpath.substr(ns_pos + 1);
  } else {
    name = xpath;
  }
  return name == name_;
}
}  // namespace polycube::polycubed::Rest::Resources::Body
