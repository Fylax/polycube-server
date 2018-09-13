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
#include "../../include/Resources/Cube.h"
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include "../../include/Resources/CubeManager.h"
#include "../../include/Validators/InSetValidator.h"
#include "../../include/Server/ResponseGenerator.h"

Cube::Cube(const std::string& name, const std::string& restEndpoint):
    ParentResource(name, restEndpoint, nullptr,
                   std::vector<PathParamField>{PathParamField{
                       std::string{'/'} + name_,
                       InSetValidator::CreateWithInSetValidator()}}) {}

void Cube::post(const Request& request, ResponseWriter response) {
  using nlohmann::detail::value_t;
  nlohmann::json body = nlohmann::json::parse(request.body());

  if (CubeManager::CreateCube(body["name"])) {
    auto val = std::static_pointer_cast<InSetValidator>(
        fields_[0].Validators()[0]);
    val->AddValue(body["name"]);
    ResponseGenerator::Generate(std::vector<Response>{{kCreated, ""}},
                                std::move(response));
  } else {
    ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kDataExists, ""}},
        std::move(response));
  }
}
