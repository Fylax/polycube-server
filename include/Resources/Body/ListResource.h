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
#include <vector>
#include "../Endpoint/PathParamField.h"

namespace polycube::polycubed::Rest::Resources::Body {
class ListResource : public virtual ParentResource {
 public:
  ListResource(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::vector<std::pair<
          std::string, std::vector<std::shared_ptr<Validators::Validator>>>>
          &&keys);

  bool ValidateXPath(const std::string &xpath) const final;

 protected:
  /** Used by derived class: no explicit virtual base initialization */
  explicit ListResource(std::vector<std::pair<
      std::string, std::vector<std::shared_ptr<Validators::Validator>>>>
                        &&keys);

  const std::vector<std::pair<
      std::string, std::vector<std::shared_ptr<Validators::Validator>>>>
      keys_;
};
}  // namespace polycube::polycubed::Rest::Resources::Body