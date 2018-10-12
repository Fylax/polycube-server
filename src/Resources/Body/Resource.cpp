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

#include "Body/Resource.h"

namespace polycube::polycubed::Rest::Resources::Body {
Resource::Resource(std::string &&name, std::string &&module,
                   std::shared_ptr<ParentResource> &&parent)
    : name_(std::move(name)),
      parent_(std::move(parent)),
      module_(std::move(module)) {}

const std::string &Resource::Name() const {
  return name_;
}

const std::string &Resource::ModuleName() const {
  return module_;
}
}  // namespace polycube::polycubed::Rest::Resources::Body
