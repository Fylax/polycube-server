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
#include "../../include/Server/RestServer.h"

Cube::Cube(const std::string& name, const std::string& base_address):
    ParentResource(name, name, base_address + name + "/:" + name + '/', nullptr,
                   std::vector<PathParamField>{PathParamField{
                       name, InSetValidator::CreateWithInSetValidator()}}),
                       body_rest_endpoint_(base_address + name + '/') {
  using Pistache::Rest::Routes::bind;
  auto router = RestServer::Router();
  router->post(body_rest_endpoint_, bind(&Cube::post_body, this));
}

Cube::~Cube() {
  using Pistache::Http::Method;
  auto router = RestServer::Router();
  router->removeRoute(Method::Post, body_rest_endpoint_);
}

void Cube::post_body(const Request& request, ResponseWriter response) {
  using nlohmann::detail::value_t;
  nlohmann::json body = nlohmann::json::parse(request.body());

  if (body.count("name") == 0) {
    ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kMissingAttribute, "name"}},
        std::move(response));
    return;
  }
  create(body["name"], std::move(response));
}

void Cube::post(const Request& request, ResponseWriter response) {
  auto name = request.param(std::string{':'} + name_).as<std::string>();
  create(name, std::move(response));
}

void Cube::create(const std::string& name, ResponseWriter&& response) {
  if (CubeManager::GetInstance().CreateCube(name)) {
    auto val = std::static_pointer_cast<InSetValidator>(
        fields_[0].Validators()[0]);
    val->AddValue(name);
    ResponseGenerator::Generate(std::vector<Response>{{kCreated, ""}},
                                std::move(response));
  } else {
    ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kDataExists, ""}},
        std::move(response));
  }
}
