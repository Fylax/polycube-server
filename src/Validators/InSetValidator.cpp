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
#include "../../include/Validators/InSetValidator.h"
#include <string>

InSetValidator::InSetValidator(): invalid_values_() { }



bool InSetValidator::Validate(const std::string& value) const {
  return invalid_values_.count(value) == 1;
}

void InSetValidator::AddValue(const std::string& value) {
invalid_values_.insert(value);
}

void InSetValidator::RemoveValue(const std::string& value) {
invalid_values_.erase(value);
}

std::vector<std::shared_ptr<Validator>> InSetValidator::CreateWithInSetValidator() {
  return std::vector<std::shared_ptr<Validator>>{
    std::static_pointer_cast<Validator>(
      std::make_shared<InSetValidator>()
  )};
}
