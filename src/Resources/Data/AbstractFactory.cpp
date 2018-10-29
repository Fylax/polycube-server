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
#include "../../../include/Resources/Data/AbstractFactory.h"
#include "../../../include/Resources/Data/Lib/ConcreteFactory.h"

namespace polycube::polycubed::Rest::Resources::Data {
std::unique_ptr<AbstractFactory> AbstractFactory::Concrete(
    const nlohmann::json &request_body) {
  if (request_body.count("protocol") == 0) {
    throw std::invalid_argument("Missing protocol.");
  }
  if (request_body.count("data") == 0) {
    throw std::invalid_argument("Missing data.");
  }
  const auto &protocol = request_body.at("protocol").get<std::string>();
  const auto &data = request_body.at("data").get<std::string>();
  if (protocol == "lib") {
    return std::make_unique<Lib::ConcreteFactory>(data);
  }
}
}  // namespace polycube::polycubed::Rest::Resources::Data