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
#ifndef PARSER_EMPTYVALIDATOR_H
#define PARSER_EMPTYVALIDATOR_H

#include <string>
#include "Validator.h"

class EmptyValidator : public Validator {
 public:
  EmptyValidator();
  bool Validate(const std::string &value) const final;
  void IsPath(bool is_path);

 private:
  bool is_path_;
};

#endif  // PARSER_EMPTYVALIDATOR_H
