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

#include <memory>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "../../Server/Error.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "../../../externals/include/nlohmann/json.hpp"
#pragma GCC diagnostic pop

namespace polycube::polycubed::Rest::Resources::Body {
class ParentResource;
class ListKey;

using ListKeyValues = std::vector<std::pair<Body::ListKey, std::string>>;
using PerListKeyValues = std::stack<ListKeyValues>;

class Resource {
 public:
  Resource(std::string &&name, std::string &&module,
           const ParentResource * parent,
           bool configuration);

  Resource(const Resource&) = delete;

  Resource &operator=(const Resource&) = delete;

  virtual ~Resource() = default;

  virtual bool IsMandatory() const = 0;

  virtual bool IsConfiguration() const = 0;

  const std::string &Name() const;

  const std::string &ModuleName() const;

  virtual bool ValidateXPath(const std::string &xpath) const = 0;

  virtual std::vector<Response> BodyValidate(nlohmann::json &body,
                                             bool check_mandatory) const = 0;

  virtual void SetDefaultIfMissing(nlohmann::json &body) const = 0;

  const std::shared_ptr<ParentResource> Parent() const;

  virtual const Response ReadValue(const std::string &cube_name,
                                   PerListKeyValues &keys) const;

 protected:
  const std::string name_;
  const ParentResource * const parent_;
  const std::string module_;
  const bool configuration_;
};
}  // namespace polycube::polycubed::Rest::Resources::Body
