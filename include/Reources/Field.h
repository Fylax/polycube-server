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

#include <string>
#include <memory>
#include <vector>
#include <utility>
#include "../Validators/Validators.h"

enum class FieldType {
  kPathParam,
  kJsonBody
};

class Field {
 public:
  bool validate(const std::string& value) const;
  /**
   * @return Name-Type pair.
   */
  std::pair<std::string, FieldType> info() const;
 protected:
  Field(const std::string& name,
      const std::vector<std::shared_ptr<Validator>>& validators,
      const FieldType& type);
 private:
  const std::vector<std::shared_ptr<Validator>> validators_;
  const std::string name_;
  const FieldType type_;
};


#endif //PARSER_FIELD_H
