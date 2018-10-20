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
#include "../../../include/Resources/Body/ChoiceResource.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../../include/Resources/Body/CaseResource.h"
#include "../../../include/Validators/InSetValidator.h"

namespace polycube::polycubed::Rest::Resources::Body {
ChoiceResource::ChoiceResource(
    bool mandatory, std::unique_ptr<const std::string> &&default_case)
    : ParentResource("", "", nullptr),
      mandatory_(mandatory),
      default_case_(std::move(default_case)) {}

ChoiceResource::ChoiceResource(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent, bool mandatory,
    std::unique_ptr<const std::string> &&default_case)
    : ParentResource(std::move(name), std::move(module), std::move(parent)),
      mandatory_(mandatory),
      default_case_(std::move(default_case)) {}

std::vector<Response> ChoiceResource::BodyValidate(nlohmann::json &body,
                                                   bool check_mandatory) const {
  bool parsed_child = false;
  std::vector<Response> errors;
  for (const auto &child : children_) {
    std::vector<Response> child_errors;
    // if children is choice or case, pass whole body
    if (std::dynamic_pointer_cast<ChoiceResource>(child) != nullptr ||
        std::dynamic_pointer_cast<CaseResource>(child) != nullptr) {
      child_errors = child->BodyValidate(body, check_mandatory);
    } else {  // else pass to the correct child
      if (body.count(child->Name()) == 0) {
        if (check_mandatory && IsMandatory() && child->IsMandatory()) {
          errors.push_back({ErrorTag::kMissingAttribute, child->Name().data()});
        }
      } else {
        if (!child->IsConfiguration()) {
          errors.push_back({ErrorTag::kInvalidValue, ""});
        } else {
          child_errors = child->BodyValidate(body.at(child->Name()),
                                             check_mandatory && IsMandatory());
          errors.reserve(errors.size() + child_errors.size());
          body.erase(child->Name());
          std::copy(std::begin(child_errors), std::end(child_errors),
                    std::back_inserter(errors));
        }
      }
    }
    if (child_errors.empty()) {
      if (parsed_child && errors.empty()) {
        errors.push_back({ErrorTag::kBadAttribute, name_.data()});
      } else {
        parsed_child = true;
      }
    }
  }
  if (errors.empty() || !parsed_child) {
    errors.push_back({ErrorTag::kUnparsableChoice, nullptr});
  }
  return errors;
}

bool ChoiceResource::IsMandatory() const {
  return mandatory_ &&
         std::dynamic_pointer_cast<CaseResource>(parent_) != nullptr;
}

void ChoiceResource::SetDefaultIfMissing(nlohmann::json &body) const {
  if (default_case_ != nullptr) {
    auto def = std::find_if(
        std::begin(children_), std::end(children_),
        [this](const auto &child) { return child->Name() == *default_case_; });
    // no check if def is end because it ensured by YANG validation process
    (*def)->SetDefaultIfMissing(body);
  }
}
}  // namespace polycube::polycubed::Rest::Resources::Body
