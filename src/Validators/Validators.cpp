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
#include "../../include/Validators/Validators.h"

#include <memory>
#include <string>
#include <vector>

Validators::Validators(): validators_() {}

Validators::Validators(const std::string& name, const std::vector<
    std::shared_ptr<Validator>>& validators): validators_{
    {name, std::make_shared<const std::vector<
        std::shared_ptr<Validator>
    >>(validators)}} {}

void Validators::addValidators(const std::shared_ptr<Validators>& validators) {
  validators_.insert(validators->validators_.begin(),
      validators->validators_.end());
}

std::shared_ptr<
    const std::vector<std::shared_ptr<Validator>>
> Validators::getValidators(const std::string& name) const {
  bool exists = validators_.count(name) == 1;
  if (exists) {
    return validators_.at(name);
  }
  return std::shared_ptr<
      const std::vector<std::shared_ptr<Validator>>
  >();
}
