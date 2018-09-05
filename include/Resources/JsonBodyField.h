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
#ifndef PARSER_JSONBODYFIELD_H
#define PARSER_JSONBODYFIELD_H

#include <libyang/libyang.h>
#include "../../externals/include/nlohmann/json.hpp"

#include <string>
#include <memory>
#include <vector>
#include "Field.h"
#include "../Validators/Validator.h"

enum class JsonType {
  kInt, kUint, kString, kDecimal, kBoolean, kEmpty, kList
};

class JsonBodyField: public Field<nlohmann::json> {
 private:
  const JsonType type_;

 public:
  JsonBodyField(const std::vector<std::shared_ptr<Validator>>& validators,
                JsonType type);
  ErrorTag Validate (const nlohmann::json& value) const override;
  JsonType Type() const;

  static JsonType FromYangType(LY_DATA_TYPE type);
};

#endif  // PARSER_JSONBODYFIELD_H
