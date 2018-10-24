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
#include <utility>
#include <vector>
#include "../Server/Error.h"
#include "../Validators/Validator.h"

namespace polycube::polycubed::Rest::Resources {
template <typename T>
class Field {
 public:
  virtual ~Field() = default;

  virtual ErrorTag Validate(const T &value) const = 0;

  const std::vector<std::shared_ptr<Validators::Validator>> &Validators()
      const {
    return validators_;
  }

 protected:
  explicit Field(
      std::vector<std::shared_ptr<Validators::Validator>> &&validators)
      : validators_(std::move(validators)) {}

  const std::vector<std::shared_ptr<Validators::Validator>> validators_;
};
}  // namespace polycube::polycubed::Rest::Resources
