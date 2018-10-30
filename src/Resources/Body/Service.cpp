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
#include "../../../include/Resources/Body/Service.h"

namespace polycube::polycubed::Rest::Resources::Body {

Service::Service(): ParentResource("", "", nullptr, true) {}

Service::Service(const std::string& name):
    ParentResource(name, name, nullptr, true) {}

bool Service::ValidateXPath(const std::string& xpath) const {
  if (xpath[0] != '/')
    return false;
  auto del_pos = xpath.find('/', 1);
  auto ns_pos = xpath.find(':', 1);
  // first element MUST be fully-qualified.
  if (ns_pos < del_pos)
    return false;
  auto ns = xpath.substr(1, ns_pos);
  // resources expect XPath without leading '/' character
  auto child_xpath = xpath.substr(1);
  for (const auto& child : children_) {
    if (child->ModuleName() == ns) {
      if (child->ValidateXPath(child_xpath))
        return true;
    }
  }
  return false;
}
}
