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
#include <cmath>
#include <string>

IntegerValidator::IntegerValidator(std::int64_t lower_bound,
                                   std::int64_t upper_bound):
    lower_bound_(lower_bound), upper_bound_(upper_bound) {}

bool IntegerValidator::Validate(const std::string& value) const {
  try {
    std::size_t idx;
    auto parsed = std::stoll(value, &idx);
    if (idx != value.length()) return false;
    return parsed >= lower_bound_ && parsed <= upper_bound_;
  } catch (std::invalid_argument&) {
    return false;
  }
}

UnsignedIntegerValidator::UnsignedIntegerValidator(std::uint64_t lower_bound,
                                                   std::uint64_t upper_bound):
    lower_bound_(lower_bound), upper_bound_(upper_bound) {}

bool UnsignedIntegerValidator::Validate(const std::string& value) const {
  try {
    std::size_t idx;
    auto parsed = std::stoull(value, &idx);
    if (idx != value.length()) return false;
    return parsed >= lower_bound_ && parsed <= upper_bound_;
  } catch (std::invalid_argument&) {
    return false;
  }
}

DecimalValidator::DecimalValidator(unsigned fraction_digits):
    DecimalValidator(fraction_digits, Decimal64::Min(fraction_digits),
                     Decimal64::Max(fraction_digits)) {}

DecimalValidator::DecimalValidator(unsigned fraction_digits,
                                   const Decimal64& lower_bound,
                                   const Decimal64& upper_bound):
    fraction_digits_(fraction_digits), lower_bound_(lower_bound),
    upper_bound_(upper_bound) {}

bool DecimalValidator::Validate(const std::string& value) const {
  try {
    std::size_t idx;
    auto numerator = std::stoll(value, &idx);

    if (idx == value.length()) return true;  // integer string
    if (value.at(idx) != '.') return false;  // invalid decimal separator

    idx += 1;
    if (idx == value.length()) return true;  // no decimal values

    size_t fraction_digits = value.length() - idx;
    if (fraction_digits > fraction_digits_) return false;

    int denominator = std::stoi(value.substr(idx));
    std::int64_t shift = 1;
    for (std::size_t i = 0; i < fraction_digits; ++i) {
      shift *= 10;
    }

    auto parsed = (numerator * shift) + denominator;
    auto fixed = Decimal64(parsed, fraction_digits);
    return fixed >= lower_bound_ && fixed <= upper_bound_;
  } catch (std::invalid_argument&) {
    return false;
  }
}
