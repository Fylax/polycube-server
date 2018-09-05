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
#ifndef PARSER_NUMBERVALIDATOR_H
#define PARSER_NUMBERVALIDATOR_H

#include "Validator.h"
#include "../Types/Decimal64.h"
#include <cstdint>
#include <string>

class IntegerValidator: public Validator {
private:
  std::int64_t lower_bound_;
  std::int64_t upper_bound_;
public:
  explicit IntegerValidator(std::int64_t lower_bound = INT64_MIN,
      std::int64_t upper_bound = INT64_MAX);
  bool Validate(const std::string& value) const override;
};

class UnsignedIntegerValidator: public Validator {
private:
  std::uint64_t lower_bound_;
  std::uint64_t upper_bound_;
public:
  explicit UnsignedIntegerValidator(std::uint64_t lower_bound = 0,
                            std::uint64_t upper_bound = UINT64_MAX);
  bool Validate(const std::string& value) const override;
};

class DecimalValidator: public Validator {
  unsigned fraction_digits_;
  Decimal64 lower_bound_;
  Decimal64 upper_bound_;

public:
  explicit DecimalValidator(unsigned fraction_digits);
  DecimalValidator(unsigned fraction_digits, const Decimal64& lower_bound,
      const Decimal64& upper_bound);
  bool Validate(const std::string& value) const override;
};


#endif //PARSER_NUMBERVALIDATOR_H
