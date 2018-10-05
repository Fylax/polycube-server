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
#include <ChoiceResource.h>

#include "../../include/Resources/ChoiceResource.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "../../include/Validators/InSetValidator.h"
#include "../../include/Server/ResponseGenerator.h"

ChoiceResource::ChoiceResource(
    std::string name, std::string module, std::string rest_endpoint,
    std::shared_ptr<ParentResource> parent, bool mandatory,
    std::unique_ptr<const std::string>&& default_case):
    ParentResource(std::move(name), std::move(module), std::move(rest_endpoint),
                   std::move(parent),
                   std::vector<PathParamField>{PathParamField{
                       "case", InSetValidator::Create()}}),
    mandatory_(mandatory), children_{},
    default_case_(std::move(default_case)) {}

std::vector<Response>
ChoiceResource::Validate(const Request& request,
                         const std::string& caller_name) const {
  auto errors = ParentResource::Validate(request, caller_name);
  // if caller name is not empty, it means that the validation
  // request comes from a children resource. it is thus
  // required to check if correct choice subpath was validated.
  if (!caller_name.empty()) {
    auto choice = request.param(":case").as<std::string>();
    if (choice != caller_name) {
      errors.push_back({ErrorTag::kBadElement, caller_name.data()});
    }
  }
  return errors;
}

void ChoiceResource::AddChild(std::shared_ptr<Resource> child) {
  auto val = std::static_pointer_cast<InSetValidator>(
      fields_[0].Validators()[0]);
  val->AddValue(child->Name());
  children_.emplace(child->Name(), std::move(child));
}

void ChoiceResource::SetDefaultIfMissing(nlohmann::json& body) const {
  if (default_case_ != nullptr) {
    children_.at(*default_case_)->SetDefaultIfMissing(body);
  }
}

void ChoiceResource::CreateOrReplace(const Request& request,
                                     ResponseWriter response) {
  std::vector<Response> errors;
  if (parent_ != nullptr)
    errors = parent_->Validate(request, "");

  nlohmann::json jbody;
  if (request.body().empty()) {
    jbody = nlohmann::json::parse("{}");
  } else {
    jbody = nlohmann::json::parse(request.body());
  }

  SetDefaultIfMissing(jbody);

  auto body = ParentResource::Validate(jbody);
  errors.reserve(errors.size() + body.size());
  std::copy(std::begin(body), std::end(body),
            std::back_inserter(errors));
  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({ErrorTag::kCreated, ""});
  }
  ResponseGenerator::Generate(std::move(errors), std::move(response));
}
