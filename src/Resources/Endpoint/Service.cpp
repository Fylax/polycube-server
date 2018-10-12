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
#include "Service.h"

#include "../../Server/ResponseGenerator.h"
#include "../../Server/RestServer.h"
#include "../CubeManager.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
Service::Service(const std::string &name, std::string base_address)
    : Body::ParentResource(name, name, nullptr),
      ParentResource(base_address + name + "/:cube_name/"),
      Body::Service(),
      body_rest_endpoint_(base_address + name + '/'),
      path_param_{} {
  using Pistache::Rest::Routes::bind;
  auto router = Server::RestServer::Router();
  router->get(body_rest_endpoint_, bind(&Service::get_body, this));
  router->post(body_rest_endpoint_, bind(&Service::post_body, this));
  router->patch(body_rest_endpoint_, bind(&Service::patch_body, this));
}

Service::~Service() {
  using Pistache::Http::Method;
  auto router = Server::RestServer::Router();
  router->removeRoute(Method::Post, body_rest_endpoint_);
}

std::vector<Response> Service::RequestValidate(
    const Request &request,
    [[maybe_unused]] const std::string &caller_name) const {
  std::vector<Response> errors;
  if (!path_param_.Validate(request.param(":cube_name").as<std::string>())) {
    errors.push_back({ErrorTag::kBadElement, ":cube_name"});
  }
  return errors;
}

void Service::CreateReplaceUpdate(const std::string &name, nlohmann::json body,
                                  ResponseWriter response, bool replace,
                                  bool check_mandatory) {
  if (CubeManager::GetInstance().CreateCube(name) || replace) {
    auto errors = BodyValidate(body, check_mandatory);
    if (errors.empty()) {
      // TODO call user code
      path_param_.AddValue(name);
      errors.push_back({ErrorTag::kCreated, ""});
    }
    Server::ResponseGenerator::Generate(std::move(errors), std::move(response));
  } else {
    Server::ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kDataExists, ""}},
        std::move(response));
  }
}

void Service::get_body(const Request &request, ResponseWriter response) {
  if (!request.body().empty()) {
    Server::ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kOperationNotSupported, ""}},
        std::move(response));
    return;
  }
  for (const auto &name : path_param_.Values()) {
    // TODO call user code
  }
  // TODO data is fetched from lib
  Server::ResponseGenerator::Generate(
      std::vector<Response>{{ErrorTag::kOk, ""}}, std::move(response));
}

void Service::post_body(const Request &request, ResponseWriter response) {
  nlohmann::json body = nlohmann::json::parse(request.body());

  if (body.count("name") == 0) {
    Server::ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kMissingAttribute, "name"}},
        std::move(response));
    return;
  }
  CreateReplaceUpdate(body["name"].get<std::string>(), body,
                      std::move(response), false, true);
}

void Service::post(const Request &request, ResponseWriter response) {
  auto name = request.param(":cube_name").as<std::string>();
  CreateReplaceUpdate(name, nlohmann::json::parse(request.body()),
                      std::move(response), false, true);
}

void Service::put(const Request &request, ResponseWriter response) {
  auto name = request.param(":cube_name").as<std::string>();
  CreateReplaceUpdate(name, nlohmann::json::parse(request.body()),
                      std::move(response), true, true);
}

void Service::patch(const Request &request, ResponseWriter response) {
  auto name = request.param(":cube_name").as<std::string>();
  CreateReplaceUpdate(name, nlohmann::json::parse(request.body()),
                      std::move(response), true, false);
}

void Service::patch_body(const Request &request, ResponseWriter response) {
  nlohmann::json body = nlohmann::json::parse(request.body());

  if (body.count("name") == 0) {
    Server::ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kMissingAttribute, "name"}},
        std::move(response));
    return;
  }
  CreateReplaceUpdate(body["name"].get<std::string>(), body,
                      std::move(response), true, false);
}
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
