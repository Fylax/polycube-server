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

#include <memory>
#include <string>

#include "../Endpoint/AbstractFactory.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "../../../externals/include/nlohmann/json.hpp"
#pragma GCC diagnostic pop

namespace polycube::polycubed::Rest::Resources::Data {
class AbstractFactory : Endpoint::AbstractFactory {
 public:
  static std::unique_ptr<AbstractFactory> Concrete(
      const nlohmann::json &request_body);
  virtual const std::string Yang() const = 0;
};
}  // namespace polycube::polycubed::Rest::Resources::Data