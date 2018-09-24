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

#include "../../include/Resources/Resource.h"

#include <pistache/router.h>
#include <memory>
#include <string>
#include <vector>

#include "../../include/Server/Error.h"

Resource::Resource(const std::string& name, const std::string& module,
                   const std::string& rest_endpoint,
                   const std::shared_ptr<ParentResource>& parent):
    name_(name), rest_endpoint_(rest_endpoint), parent_(parent),
    module_(module) {}

const std::string& Resource::Name() const {
  return name_;
}

const std::string& Resource::ModuleName() const {
  return module_;
}

const std::string& Resource::Endpoint() const {
  return rest_endpoint_;
}
