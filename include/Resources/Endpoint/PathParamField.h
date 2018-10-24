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

#include <pistache/router.h>

#include <memory>
#include <string>
#include <vector>

#include "../../Validators/Validator.h"
#include "../Field.h"

namespace polycube::polycubed::Rest::Resources {
template class Field<Pistache::Rest::Request>;
}

namespace polycube::polycubed::Rest::Resources::Endpoint {
class PathParamField : public Resources::Field<Pistache::Rest::Request> {
 private:
  const std::string name_;

 public:
  PathParamField(
      const std::string &name,
      std::vector<std::shared_ptr<Validators::Validator>> validators);

  const std::string &Name() const;

  ErrorTag Validate(const Pistache::Rest::Request &value) const final;

  bool Validate(const std::string &value) const;
};
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
