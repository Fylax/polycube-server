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
#ifndef PARSER_INSETVALIDATOR_H
#define PARSER_INSETVALIDATOR_H

#include "Validator.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

class InSetValidator: public Validator {
public:
  static std::vector<std::shared_ptr<Validator>> CreateWithInSetValidator();
  InSetValidator();
  bool Validate(const std::string& value) const override;
  void AddValue(const std::string& value);
  void RemoveValue(const std::string& value);
private:
  std::unordered_set<std::string> invalid_values_;
};


#endif //PARSER_INSETVALIDATOR_H
