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
#include <string>
#include <unordered_map>
#include <vector>
#include "../../Server/Error.h"
#include "JsonBodyField.h"
#include "Resource.h"

namespace polycube::polycubed::Rest::Resources::Body {
class LeafResource : public Resource {
 public:
  LeafResource(std::string name, std::string module,
               std::shared_ptr<ParentResource> parent,
               std::unique_ptr<JsonBodyField> &&field, bool configuration,
               bool mandatory,
               std::unique_ptr<const std::string> &&default_value);

  ~LeafResource() override = default;

  std::vector<Response> BodyValidate(nlohmann::json &body,
                                     bool check_mandatory) const override;

  bool IsMandatory() const final;

  bool IsConfiguration() const final;

  void SetDefaultIfMissing(nlohmann::json &body) const override;

  bool ValidateXPath(const std::string &xpath) const final;

 protected:
  const std::unique_ptr<JsonBodyField> field_;
  const bool configuration_;
  const bool mandatory_;
  const std::unique_ptr<const std::string> default_;
};
}  // namespace polycube::polycubed::Rest::Resources::Body
