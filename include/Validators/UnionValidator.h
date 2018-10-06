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
#ifndef PARSER_UNIONVALIDATOR_H
#define PARSER_UNIONVALIDATOR_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "../../externals/include/nlohmann/json.hpp"
#pragma GCC diagnostic pop

#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <vector>
#include "Validator.h"

class UnionValidator : public Validator {
 public:
  UnionValidator();
  void AddType(std::type_index type,
               const std::vector<std::shared_ptr<Validator>> &validators);
  /**
   * Validates a union with respect to RFC 7950 and 7951.
   * In particular it firstly JSON type, then it validates in union provided
   * order between acceptable types.
   * @param value
   * @return
   */
  bool Validate(const std::string &value) const;

 private:
  std::map<std::type_index, std::vector<std::shared_ptr<Validator>>> map_;
};

#endif  // PARSER_UNIONVALIDATOR_H
