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
#include "../../include/Validators/EnumValidator.h"

#include <string>
#include <unordered_set>
#include <utility>

EnumValidator::EnumValidator() : values_{} {}

void EnumValidator::AddEnum(const std::string &value) {
  values_.emplace(value);
}

bool EnumValidator::Validate(const std::string &value) const {
  return values_.count(value) == 1;
}
