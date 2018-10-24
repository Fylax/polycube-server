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

#include <pistache/router.h>

#include <memory>
#include <shared_mutex>
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

class Resource {
 public:
  Resource(std::string &&name, std::string &&module,
           std::shared_ptr<ParentResource> &&parent);

  virtual ~Resource() = default;

  virtual bool IsMandatory() const = 0;

  virtual bool IsConfiguration() const = 0;

  const std::string &Name() const;

  const std::string &ModuleName() const;

  virtual bool ValidateXPath(const std::string &xpath) const = 0;

  virtual std::vector<Response> BodyValidate(nlohmann::json &body,
                                             bool check_mandatory) const = 0;

  virtual void SetDefaultIfMissing(nlohmann::json &body) const = 0;

  std::shared_ptr<ParentResource> Parent() const;

  virtual const nlohmann::json Value(const std::string &cube_name) = 0;

  virtual void Value(nlohmann::json value, bool replace) = 0;

 protected:
  const std::string name_;
  std::shared_ptr<ParentResource> parent_;
  const std::string module_;
};
}  // namespace polycube::polycubed::Rest::Resources::Body
