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

#include <libyang/libyang.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "../../../externals/include/nlohmann/json.hpp"
#pragma GCC diagnostic pop

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <vector>
#include "../../Validators/Validator.h"
#include "../Field.h"

namespace polycube::polycubed::Rest::Resources::Body {

class JsonBodyField : public Field<nlohmann::json> {
 public:
  JsonBodyField();

  JsonBodyField(
      LY_DATA_TYPE type,
      std::vector<std::shared_ptr<Validators::Validator>> &&validators);

  ErrorTag Validate(const nlohmann::json &value) const final;

  static const std::unordered_set<std::type_index> AcceptableTypes(
      nlohmann::detail::value_t type);

 private:
  const std::unordered_set<nlohmann::detail::value_t> allowed_types_;

  static const std::unordered_set<nlohmann::detail::value_t> FromYangType(
      LY_DATA_TYPE type);
};
}  // namespace polycube::polycubed::Rest::Resources::Body
