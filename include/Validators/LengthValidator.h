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
#ifndef PARSER_LENGTHVALIDATOR_H
#define PARSER_LENGTHVALIDATOR_H

#include "Validator.h"

#include <string>
#include <unordered_map>

class LengthValidator : public Validator {
 public:
  LengthValidator(bool binary);
  void AddRange(std::uint64_t min, std::uint64_t max);
  void AddExact(std::uint64_t exact);
  void AddRanges(std::unordered_map<std::uint64_t, std::uint64_t> ranges);
  bool Validate(const std::string &value) const final;

 private:
  const bool binary_;
  std::unordered_map<std::uint64_t, std::uint64_t> ranges_;
};

#endif  // PARSER_LENGTHVALIDATOR_H
