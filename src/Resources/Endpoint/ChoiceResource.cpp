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
#include "../../../include/Resources/Endpoint/ChoiceResource.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace polycube::polycubed::Rest::Resources::Endpoint {
ChoiceResource::ChoiceResource(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent, bool mandatory,
    std::unique_ptr<const std::string> &&default_case)
    : Body::ParentResource(std::move(name), std::move(module),
                           std::move(parent), false),
      ParentResource(),
      Body::ChoiceResource(mandatory, std::move(default_case)),
      choice_{} {}

void ChoiceResource::AddChild(std::shared_ptr<Body::Resource> child) {
  Body::ParentResource::AddChild(child);
  choice_.AddValue(child->Name());
}
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
