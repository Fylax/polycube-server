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
#include "../../include/Error.h"

ParentResource::ParentResource(const std::string& name,
                               const std::shared_ptr<Pistache::Rest::Router>& router,
                               const std::string& restEndpoint,
                               const std::shared_ptr<ParentResource>& parent,
                               bool container_presence)
    : Resource(name, router, restEndpoint, parent), children_(),
    container_presence_(container_presence) {}

Response ParentResource::Validate(const Pistache::Rest::Request& request) const {
  using Pistache::Http::Code;

  if (parent_ != nullptr) return parent_->Validate(request);

  for (const auto& field : fields_) {
    auto valid = field.Validate(request);
    if (!valid) return {ErrorTag::kBadElement, field.Name().c_str()};
  }

  return {ErrorTag::kOk, ""};
}

void ParentResource::AddChild(const std::shared_ptr<Resource>& child) {
  children_.push_back(child);
}

bool ParentResource::IsMandatory() const {
  if (!container_presence_) return false;
  for (const auto& child : children_) {
    if (child->IsMandatory()) return true;
  }
  return false;
}
