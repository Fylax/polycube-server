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

#include <string>
#include "Validator.h"
namespace polycube::polycubed::Rest::Validators {
class XPathValidator : public Validator {
 public:
  /**
   * Creates a XPathValidator.
   * @param context The namespace (module name) where the element
   * to validate (i.e. an ~instance-identified~) resides.
   */
  explicit XPathValidator(const std::string &context);

  bool Validate(const std::string &value) const final;

 private:
  const std::string context_;
};
}  // namespace polycube::polycubed::Rest::Validators
