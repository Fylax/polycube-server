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
#ifndef PARSER_FIELD_H
#define PARSER_FIELD_H

#include <pistache/router.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "../../externals/include/nlohmann/json.hpp"
#pragma GCC diagnostic pop

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "../Server/Error.h"
#include "../Validators/Validator.h"

template <typename T>
class Field {
 public:
  virtual ~Field() = default;
  virtual ErrorTag Validate(const T &value) const = 0;

  const std::vector<std::shared_ptr<Validator>> &Validators() const {
    return validators_;
  }

 protected:
  explicit Field(std::vector<std::shared_ptr<Validator>> &&validators)
      : validators_(std::move(validators)) {}

  const std::vector<std::shared_ptr<Validator>> validators_;
};

template class Field<Pistache::Rest::Request>;

template class Field<nlohmann::json>;

#endif  // PARSER_FIELD_H
