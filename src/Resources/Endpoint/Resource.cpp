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
#include "../../../include/Resources/Endpoint/Resource.h"

#include <string>
#include <Resource.h>


namespace polycube::polycubed::Rest::Resources::Endpoint {

Resource::Resource(std::string&& rest_endpoint)
    : rest_endpoint_(rest_endpoint) {}

const std::string& Resource::Endpoint() const {
  return rest_endpoint_;
}

Operation Resource::OperationType(bool update, bool check_mandatory) {
  if (!update) {
    return Operation::kCreate;
  } else {
    if (check_mandatory) {
      return Operation::kReplace;
    } else {
      return Operation::kUpdate;
    }
  }
}
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
