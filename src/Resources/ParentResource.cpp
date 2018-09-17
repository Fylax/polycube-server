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
#include "../../include/Resources/ParentResource.h"
#include <algorithm>
#include <pistache/router.h>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include "../../include/Server/Error.h"
#include "../../include/Server/ResponseGenerator.h"

ParentResource::ParentResource(const std::string& name,
                               const std::string& restEndpoint,
                               const std::shared_ptr<ParentResource>& parent,
                               std::vector<PathParamField>&& fields,
                               bool container_presence)
    : Resource(name, restEndpoint, parent), fields_(std::move(fields)),
      children_(), container_presence_(container_presence) {}

std::vector<Response>
ParentResource::Validate(const Pistache::Rest::Request& request) const {
  std::vector<Response> errors;
  if (parent_ != nullptr) {
    errors = parent_->Validate(request);
    if (errors[0].error_tag == kOk) {
      errors.pop_back();
    }
  }

  for (const auto& field : fields_) {
    auto valid = field.Validate(request);
    if (!valid) {
      errors.push_back({ErrorTag::kBadElement, field.Name().c_str()});
    }
  }
  return errors;
}

std::vector<Response>
ParentResource::Validate(const nlohmann::json& body) const {
  std::vector<Response> errors;
  for (const auto& child : children_) {
    if (body.count(child->Name()) == 0) {
      if (child->IsMandatory()) {
        errors.push_back({ErrorTag::kMissingAttribute, child->Name().c_str()});
      }
    } else {
      auto child_errors = child->Validate(body.at(child->Name()));
      errors.reserve(errors.size() + child_errors.size());
      std::copy(std::begin(child_errors), std::end(child_errors),
                std::back_inserter(errors));
    }
  }
  return errors;
}

void ParentResource::AddChild(std::shared_ptr<Resource> child) {
  children_.push_back(std::move(child));
}

bool ParentResource::IsMandatory() const {
  if (container_presence_) return true;
  for (const auto& child : children_) {
    if (child->IsMandatory()) return true;
  }
  return false;
}

void ParentResource::get(const Request& request, ResponseWriter response) {
  auto resp = parent_->Validate(request);
  // TODO: call user code and merge responses
  if (resp.empty()) {
    resp.push_back({ErrorTag::kOk, ""});
  }
  ResponseGenerator::Generate(std::move(resp), std::move(response));
}

void ParentResource::post(const Request& request, ResponseWriter response) {
  auto resp = parent_->Validate(request);
  auto body = Validate(nlohmann::json::parse(request.body()));
  resp.reserve(resp.size() + body.size());
  std::copy(std::begin(body), std::end(body),
            std::back_inserter(resp));
  // TODO: call user code and merge responses
  if (resp.empty()) {
    resp.push_back({ErrorTag::kCreated, ""});
  }
  ResponseGenerator::Generate(std::move(resp), std::move(response));

}

void ParentResource::put(const Request& request, ResponseWriter response) {
  post(request, std::move(response));
}

void ParentResource::patch(const Request& request, ResponseWriter response) {
  auto resp = parent_->Validate(request);
  auto body = nlohmann::json::parse(request.body());

  for (const auto& child : children_) {
    if (body.count(child->Name()) != 0) {
      auto child_errors = child->Validate(body.at(child->Name()));
      resp.reserve(resp.size() + child_errors.size());
      std::copy(std::begin(child_errors), std::end(child_errors),
                std::back_inserter(resp));
    }
  }
  // TODO: call user code and merge responses
  if (resp.empty()) {
    resp.push_back({ErrorTag::kOk, ""});
  }
  ResponseGenerator::Generate(std::move(resp), std::move(response));
}
