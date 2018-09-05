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
#include "../../include/Types/Decimal64.h"

Decimal64::Decimal64(std::int64_t normalized_value,
    unsigned int base_10_exponent):
value_(normalized_value), fraction_digits_(-base_10_exponent) { }


bool Decimal64::operator<(const Decimal64& other) const {
  return value_ < other.value_;
}

bool Decimal64::operator>(const Decimal64& other) const {
  return other < *this;
}

bool Decimal64::operator<=(const Decimal64& other) const {
  return !(*this > other);
}

bool Decimal64::operator>=(const Decimal64& other) const {
  return !(*this < other);
}

Decimal64 Decimal64::Min(unsigned int base_10_exponent) {
  return Decimal64(-9223372036854775807 - 1, base_10_exponent);
}

Decimal64 Decimal64::Max(unsigned int base_10_exponent) {
  return Decimal64(9223372036854775807, base_10_exponent);
}


