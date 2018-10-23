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
#include <typeindex>
#include <vector>
#include <libyang/tree_schema.h>

#include "../../Validators/Validator.h"

namespace polycube::polycubed::Rest::Resources::Body {
class ListKey {
 public:
  ListKey(LY_DATA_TYPE type, std::string name,
          std::vector<std::shared_ptr<Validators::Validator>> &&validators);

  const std::type_index Type() const;

  const std::string Name() const;

  const std::vector<std::shared_ptr<Validators::Validator>> &Validators() const;

 private:
  const std::type_index type_;
  const std::string name_;
  const std::vector<std::shared_ptr<Validators::Validator>> validators_;
  static std::type_index FromYang(LY_DATA_TYPE type);
};
}  // namespace polycube::polycubed::Rest::Resources::Body