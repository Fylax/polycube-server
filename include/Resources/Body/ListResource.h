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
#pragma once

#include "ParentResource.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace polycube::polycubed::Rest::Resources::Body {
class ListKey;

class ListResource : public virtual ParentResource {
 public:
  ListResource(std::string name, std::string module,
               std::shared_ptr<ParentResource> parent,
               std::vector<ListKey> &&keys);

  std::vector<Response> MultipleBodyValidate(nlohmann::json &body,
                                             bool update) const;

  bool ValidateXPath(const std::string &xpath) const final;

  bool ValidateKeys(std::unordered_map<std::string, std::string> keys) const;

  const Response ReadValue(
      const std::string& cube_name, PerListKeyValues& keys) const override;

  virtual const Response ReadWhole(const std::string &cube_name,
                                   PerListKeyValues &keys) const;

 protected:
  /** Used by derived class: no explicit virtual base initialization */
  explicit ListResource(std::vector<ListKey> &&keys);

  const std::vector<ListKey> keys_;
};
}  // namespace polycube::polycubed::Rest::Resources::Body
