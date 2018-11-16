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

namespace polycube::polycubed::Rest::Validators {
template <typename T>
NumberValidator<T>::NumberValidator(T lower_bound, T upper_bound)
    : ranges_{{lower_bound, upper_bound}} {}

template <typename T>
void NumberValidator<T>::AddRange(T lower_bound, T upper_bound) {
  ranges_.emplace(lower_bound, upper_bound);
}

template <typename T>
void NumberValidator<T>::AddExact(T exact) {
  ranges_.emplace(exact, exact);
}

template <typename T>
void NumberValidator<T>::AddRanges(std::unordered_map<T, T> ranges) {
  ranges_.merge(ranges);
}

template <typename T>
bool NumberValidator<T>::Validate(const std::string &value) const {
  using boost::lexical_cast;
  using boost::numeric_cast;
  try {
    if constexpr (std::is_same<T, int8_t>() || std::is_same<T, uint8_t>()) {
      return Validate(numeric_cast<T>(lexical_cast<int>(value.data(), value.length())));
    }
    return Validate(lexical_cast<T>(value));
  } catch (const boost::bad_lexical_cast &) {
    return false;
  }
}

template <typename T>
bool NumberValidator<T>::Validate(T parsed) const {
  unsigned falses = 0;
  for (const auto &range : ranges_) {
    if (parsed >= range.first && parsed <= range.second)
      falses += 1;
  }
  return falses != ranges_.size();
}

DecimalValidator::DecimalValidator(std::uint8_t fraction_digits)
    : DecimalValidator(fraction_digits, Decimal64::Min(fraction_digits),
                       Decimal64::Max(fraction_digits)) {}

DecimalValidator::DecimalValidator(std::uint8_t fraction_digits,
                                   const Decimal64 &lower_bound,
                                   const Decimal64 &upper_bound)
    : NumberValidator(lower_bound, upper_bound),
      fraction_digits_(fraction_digits) {}

bool DecimalValidator::Validate(const std::string &value) const {
  try {
    auto parsed = boost::lexical_cast<Decimal64>(value);
    if (parsed.FractionDigits() > fraction_digits_)
      return false;

    return NumberValidator::Validate(parsed);
  } catch (const boost::bad_lexical_cast &) {
    return false;
  }
}
}  // namespace polycube::polycubed::Rest::Validators
