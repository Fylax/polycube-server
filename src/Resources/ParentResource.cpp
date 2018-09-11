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
#include <pistache/router.h>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include "../../include/Server/Error.h"

ParentResource::ParentResource(const std::string& name,
                               const std::shared_ptr<Pistache::Rest::Router>& router,
                               const std::string& restEndpoint,
                               const std::shared_ptr<ParentResource>& parent,
                               std::vector<PathParamField>&& fields,
                               bool container_presence)
    : Resource(name, router, restEndpoint, parent), fields_(std::move(fields)),
    children_(), container_presence_(container_presence) {}

ParentResource::ParentResource(const ParentResource& other):
Resource(other.name_, other.router_, other.restEndpoint_, other.parent_),
fields_(other.fields_), container_presence_(other.container_presence_) {}

std::vector<Response> ParentResource::Validate(const Pistache::Rest::Request& request) const {
  using Pistache::Http::Code;

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

  if (errors.empty()) {
    errors.push_back({ErrorTag::kOk, ""});
  }
  return errors;
}

void ParentResource::AddChild(std::unique_ptr<Resource>&& child) {
  children_.push_back(std::move(child));
}

bool ParentResource::IsMandatory() const {
  if (!container_presence_) return false;
  for (const auto& child : children_) {
    if (child->IsMandatory()) return true;
  }
  return false;
}
