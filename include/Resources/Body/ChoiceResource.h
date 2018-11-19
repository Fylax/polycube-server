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
#include <string>
#include <unordered_map>
#include <vector>

#include "ParentResource.h"

namespace polycube::polycubed::Rest::Resources::Body {
class ChoiceResource: public virtual ParentResource {
public:
  ChoiceResource(std::string name, std::string module,
                 const ParentResource * parent, bool mandatory,
                 std::unique_ptr<const std::string>&& default_case);

  std::vector<Response> BodyValidate(nlohmann::json& body,
                                     bool check_mandatory) const final;

  using ParentResource::AddChild;

  bool IsMandatory() const final;

  void SetDefaultIfMissing(nlohmann::json& body) const final;

protected:
  /** Used in derived class: no virtual base initialization */
  ChoiceResource(bool mandatory,
                 std::unique_ptr<const std::string>&& default_case);

private:
  const bool mandatory_;
  const std::unique_ptr<const std::string> default_case_;
};
}  // namespace polycube::polycubed::Rest::Resources::Body
