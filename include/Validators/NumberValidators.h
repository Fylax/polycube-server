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
#include <limits>
#include <string>
#include <map>
#include "../Types/Decimal64.h"

template <typename T>
class NumberValidator: public Validator {
public:
  void AddRange(T lower_bound, T upper_bound);
  void AddExact(T exact);
  bool Validate(const std::string& value) const override;
protected:
  bool Validate(T parsed) const;
  NumberValidator(T lower_bound, T upper_bound);
  explicit NumberValidator(T exact);
  std::map<T, T> ranges_;
};

class Int8Validator: public NumberValidator<std::int8_t> {
public:
  Int8Validator(std::int8_t lower_bound = std::numeric_limits<std::int8_t>::min(),
      std::int8_t upper_bound = std::numeric_limits<std::int8_t>::max());
  explicit Int8Validator(std::int8_t exact);
};

class Int16Validator: public NumberValidator<std::int16_t> {
public:
  Int16Validator(std::int16_t lower_bound = std::numeric_limits<int16_t>::min(),
                std::int16_t upper_bound = std::numeric_limits<std::int16_t>::max());
  explicit Int16Validator(std::int16_t exact);
};

class Int32Validator: public NumberValidator<std::int32_t> {
public:
  Int32Validator(std::int32_t lower_bound = std::numeric_limits<int32_t>::min(),
                 std::int32_t upper_bound = std::numeric_limits<std::int32_t>::max());
  explicit Int32Validator(std::int32_t exact);
};

class Int64Validator: public NumberValidator<std::int64_t> {
public:
  Int64Validator(std::int64_t lower_bound = std::numeric_limits<std::int64_t>::min(),
                 std::int64_t upper_bound = std::numeric_limits<std::int64_t>::max());
  explicit Int64Validator(std::int64_t exact);
};

class UInt8Validator: public NumberValidator<std::uint8_t> {
public:
  UInt8Validator(std::uint8_t lower_bound = std::numeric_limits<std::uint8_t>::min(),
                std::uint8_t upper_bound = std::numeric_limits<std::uint8_t>::max());
  explicit UInt8Validator(std::uint8_t exact);
};

class UInt16Validator: public NumberValidator<std::uint16_t> {
public:
  UInt16Validator(std::uint16_t lower_bound = std::numeric_limits<std::uint16_t>::min(),
                 std::uint16_t upper_bound = std::numeric_limits<std::uint16_t>::max());
  explicit UInt16Validator(std::uint16_t exact);
};

class UInt32Validator: public NumberValidator<std::uint32_t> {
public:
  UInt32Validator(std::uint32_t lower_bound = std::numeric_limits<std::uint32_t>::min(),
                 std::uint32_t upper_bound = std::numeric_limits<std::uint32_t>::max());
  explicit UInt32Validator(std::uint32_t exact);
};

class UInt64Validator: public NumberValidator<std::uint64_t> {
public:
  UInt64Validator(std::uint64_t lower_bound = std::numeric_limits<std::uint64_t>::min(),
                 std::uint64_t upper_bound = std::numeric_limits<std::uint64_t>::min());
  explicit UInt64Validator(std::uint64_t exact);
};

class DecimalValidator: public NumberValidator<Decimal64> {
public:
  explicit DecimalValidator(unsigned fraction_digits);
  DecimalValidator(unsigned fraction_digits, const Decimal64& lower_bound,
      const Decimal64& upper_bound);
  DecimalValidator(unsigned fraction_digits, const Decimal64& exact);
  bool Validate(const std::string& value) const override;

private:
  unsigned fraction_digits_;
};


#endif //PARSER_NUMBERVALIDATOR_H
