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
#include <string>
#include <unordered_map>
#include "../Types/Decimal64.h"

template<typename T>
class NumberValidator: public Validator {
public:
  void AddRange(T lower_bound, T upper_bound);

  void AddExact(T exact);

  bool Validate(const std::string& value) const override;

  NumberValidator(T lower_bound, T upper_bound);

protected:
  bool Validate(T parsed) const;

  std::unordered_map<T, T> ranges_;
};

class DecimalValidator: public NumberValidator<Decimal64> {
public:
  explicit DecimalValidator(unsigned fraction_digits);

  DecimalValidator(unsigned fraction_digits, const Decimal64& lower_bound,
                   const Decimal64& upper_bound);

  bool Validate(const std::string& value) const override;

private:
  unsigned fraction_digits_;
};

template
class NumberValidator<std::int8_t>;

template
class NumberValidator<std::uint8_t>;

template
class NumberValidator<std::int16_t>;

template
class NumberValidator<std::uint16_t>;

template
class NumberValidator<std::int32_t>;

template
class NumberValidator<std::uint32_t>;

template
class NumberValidator<std::int64_t>;

template
class NumberValidator<std::uint64_t>;

#endif //PARSER_NUMBERVALIDATOR_H
