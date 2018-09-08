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
#include "../../include/Validators/NumberValidators.h"

#include <boost/lexical_cast.hpp>
#include <string>

template<typename T>
void NumberValidator<T>::AddRange(T lower_bound, T upper_bound) {
  ranges_.emplace(lower_bound, upper_bound);
}

template<typename T>
void NumberValidator<T>::AddExact(T exact) {
  ranges_.emplace(exact, exact);
}

template<typename T>
bool NumberValidator<T>::Validate(const std::string& value) const {
  try {
    return Validate(boost::lexical_cast<T>(value));
  }
  catch(const boost::bad_lexical_cast &) {
    return false;
  }
}

template<typename T>
bool NumberValidator<T>::Validate(T parsed) const {
  for (const auto& range : ranges_) {
    if (parsed >= range.first && parsed <= range.second) return false;
  }
  return true;
}

template<typename T>
NumberValidator<T>::NumberValidator(T lower_bound, T upper_bound):
    ranges_{{lower_bound, upper_bound}} {}

template<typename T>
NumberValidator<T>::NumberValidator(T exact): NumberValidator(exact, exact) {}

Int8Validator::Int8Validator(std::int8_t lower_bound, std::int8_t upper_bound)
    : NumberValidator(lower_bound, upper_bound) {}

Int8Validator::Int8Validator(std::int8_t exact): NumberValidator(exact) {}

Int16Validator::Int16Validator(std::int16_t lower_bound,
                               std::int16_t upper_bound)
    : NumberValidator(lower_bound, upper_bound) {}

Int16Validator::Int16Validator(std::int16_t exact): NumberValidator(exact) {}

Int32Validator::Int32Validator(std::int32_t lower_bound,
                               std::int32_t upper_bound)
    : NumberValidator(lower_bound, upper_bound) {}

Int32Validator::Int32Validator(std::int32_t exact): NumberValidator(exact) {}

Int64Validator::Int64Validator(std::int64_t lower_bound,
                               std::int64_t upper_bound)
    : NumberValidator(lower_bound, upper_bound) {}

Int64Validator::Int64Validator(std::int64_t exact): NumberValidator(exact) {}

UInt8Validator::UInt8Validator(std::uint8_t lower_bound,
                               std::uint8_t upper_bound)
    : NumberValidator(lower_bound, upper_bound) {}

UInt8Validator::UInt8Validator(std::uint8_t exact): NumberValidator(exact) {}

UInt16Validator::UInt16Validator(std::uint16_t lower_bound,
                                 std::uint16_t upper_bound)
    : NumberValidator(lower_bound, upper_bound) {}

UInt16Validator::UInt16Validator(std::uint16_t exact): NumberValidator(exact) {}

UInt32Validator::UInt32Validator(std::uint32_t lower_bound,
                                 std::uint32_t upper_bound)
    : NumberValidator(lower_bound, upper_bound) {}

UInt32Validator::UInt32Validator(std::uint32_t exact): NumberValidator(exact) {}

UInt64Validator::UInt64Validator(std::uint64_t lower_bound,
                                 std::uint64_t upper_bound)
    : NumberValidator(lower_bound, upper_bound) {}

UInt64Validator::UInt64Validator(std::uint64_t exact): NumberValidator(exact) {}

DecimalValidator::DecimalValidator(unsigned fraction_digits):
    DecimalValidator(fraction_digits, Decimal64::Min(fraction_digits),
                     Decimal64::Max(fraction_digits)) {}

DecimalValidator::DecimalValidator(unsigned fraction_digits,
                                   const Decimal64& lower_bound,
                                   const Decimal64& upper_bound):
    NumberValidator(lower_bound, upper_bound),
    fraction_digits_(fraction_digits) {}

DecimalValidator::DecimalValidator(unsigned fraction_digits,
                                   const Decimal64& exact):
    NumberValidator(exact), fraction_digits_(fraction_digits) {}

bool DecimalValidator::Validate(const std::string& value) const {
  try {
    auto parsed = boost::lexical_cast<Decimal64>(value);
    if (parsed.FractionDigits() > fraction_digits_) return false;

    return NumberValidator::Validate(parsed);
  }
  catch(const boost::bad_lexical_cast &) {
    return false;
  }
}
