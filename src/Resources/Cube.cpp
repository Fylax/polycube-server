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

#include <pistache/router.h>

#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <Cube.h>

#include "../../include/Resources/CubeManager.h"
#include "../../include/Validators/InSetValidator.h"
#include "../../include/Server/ResponseGenerator.h"
#include "../../include/Server/RestServer.h"

Cube::Cube(const std::string& name, std::string base_address):
    ParentResource(name, name, base_address + name + "/:cube_name/", nullptr,
                   std::vector<PathParamField>{PathParamField{
                       "cube_name", InSetValidator::Create()}}),
    body_rest_endpoint_(base_address + name + '/'), mutex_{} {
  using Pistache::Rest::Routes::bind;
  auto router = RestServer::Router();
  router->get(body_rest_endpoint_, bind(&Cube::get_body, this));
  router->post(body_rest_endpoint_, bind(&Cube::post_body, this));
}

Cube::~Cube() {
  using Pistache::Http::Method;
  auto router = RestServer::Router();
  router->removeRoute(Method::Post, body_rest_endpoint_);
}

bool Cube::ValidateXPath(const std::string& xpath) const {
  if (xpath[0] != '/') return false;
  auto del_pos = xpath.find('/', 1);
  auto ns_pos = xpath.find(':', 1);
  // first element MUST be fully-qualified.
  if (ns_pos < del_pos) return false;
  auto ns = xpath.substr(1, ns_pos);
  // resources expect XPath without leading '/' character
  auto child_xpath = xpath.substr(1);
  for (const auto& child : children_) {
    if (child->ModuleName() == ns) {
      if (child->ValidateXPath(child_xpath)) return true;
    }
  }
  return false;
}

void Cube::CreateOrReplace(const std::string& name, const nlohmann::json& body,
                           ResponseWriter response, bool replace) {
  if (CubeManager::GetInstance().CreateCube(name) || replace) {
    auto errors = Validate(body);
    if (errors.empty()) {
      // TODO call user code
      auto val = std::static_pointer_cast<InSetValidator>(
          fields_[0].Validators()[0]);
      val->AddValue(name);
      errors.push_back({ErrorTag::kCreated, ""});
    }
    ResponseGenerator::Generate(std::move(errors), std::move(response));
  } else {
    ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kDataExists, ""}},
        std::move(response));
  }
}

void Cube::get_body(const Request& request, ResponseWriter response) {
  if (!request.body().empty()) {
    ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kOperationNotSupported, ""}}
    std::move(response));
    return;
  }
  auto val = std::static_pointer_cast<InSetValidator>(
      fields_[0].Validators()[0]);
  for (const auto& name : val->Values()) {
    // TODO call user code
  }
  // TODO data is fetched from lib
  ResponseGenerator::Generate(
      std::vector<Response>{{ErrorTag::kOk, ""}}
  std::move(response));
}

void Cube::post_body(const Request& request, ResponseWriter response) {
  nlohmann::json body = nlohmann::json::parse(request.body());

  if (body.count("name") == 0) {
    ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kMissingAttribute, "name"}},
        std::move(response));
    return;
  }
  CreateOrReplace(body["name"].get<std::string>(), body, std::move(response));
}

void Cube::post(const Request& request, ResponseWriter response) {
  auto name = request.param(":cube_name").as<std::string>();
  CreateOrReplace(name, nlohmann::json::parse(request.body()),
                  std::move(response));
}

void Cube::patch_body(const Request& request, ResponseWriter response) {
  nlohmann::json body = nlohmann::json::parse(request.body());

  if (body.count("name") == 0) {
    ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kMissingAttribute, "name"}},
        std::move(response));
    return;
  }

  const auto& name = body["name"].get<std::string>();
  if (CubeManager::GetInstance().ExistsCube(name)) {
    auto errors = Validate(body);
    if (errors.empty()) {
      // TODO call user code
      auto val = std::static_pointer_cast<InSetValidator>(
          fields_[0].Validators()[0]);
      val->AddValue(name);
      errors.push_back({ErrorTag::kCreated, ""});
    }
    ResponseGenerator::Generate(std::move(errors), std::move(response));
  } else {
    ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kDataMissing, ""}},
        std::move(response));
  }
}
