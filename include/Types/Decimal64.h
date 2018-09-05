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
#ifndef PARSER_DECIMAL64_H
#define PARSER_DECIMAL64_H

#include <cstdint>

class Decimal64 {
 public:
  /**
   * Stores a decimal64 YANG value in the form v=i^-n
   * @param normalized_value i
   * @param base_10_exponent n
   */
  Decimal64(std::int64_t normalized_value, unsigned int base_10_exponent);
  bool operator< (const Decimal64& other) const;
  bool operator> (const Decimal64& other) const;
  bool operator<=(const Decimal64& other) const;
  bool operator>=(const Decimal64& other) const;

  static Decimal64 Min(unsigned int base_10_exponent);
  static Decimal64 Max(unsigned int base_10_exponent);
 private:
  const std::int64_t value_;
  const unsigned int fraction_digits_;
};


#endif //PARSER_DECIMAL64_H
