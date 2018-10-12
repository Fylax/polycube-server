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
#include "../../../include/Resources/Endpoint/ParentResource.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include "../../../include/Server/ResponseGenerator.h"
#include "../../../include/Server/RestServer.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
ParentResource::ParentResource()
    : Body::ParentResource("", "", nullptr, false),
      Endpoint::Resource(""),
      has_endpoints_(false) {}

ParentResource::ParentResource(std::string rest_endpoint)
    : ParentResource("", "", std::move(rest_endpoint), nullptr, false) {}

ParentResource::ParentResource(std::string name, std::string module,
                               std::string rest_endpoint,
                               std::shared_ptr<ParentResource> parent,
                               bool container_presence)
    : Body::ParentResource(std::move(name), std::move(module),
                           std::move(parent), container_presence),
      Endpoint::Resource(std::move(rest_endpoint)),
      has_endpoints_(true) {
  using Pistache::Rest::Routes::bind;
  auto router = Server::RestServer::Router();
  router->get(rest_endpoint_, bind(&ParentResource::get, this));
  router->post(rest_endpoint_, bind(&ParentResource::post, this));
  router->put(rest_endpoint_, bind(&ParentResource::put, this));
  router->patch(rest_endpoint_, bind(&ParentResource::patch, this));
}

ParentResource::~ParentResource() {
  if (has_endpoints_) {
    using Pistache::Http::Method;
    auto router = Server::RestServer::Router();
    router->removeRoute(Method::Get, rest_endpoint_);
    router->removeRoute(Method::Post, rest_endpoint_);
    router->removeRoute(Method::Put, rest_endpoint_);
    router->removeRoute(Method::Patch, rest_endpoint_);
  }
}

std::vector<Response> ParentResource::RequestValidate(
    const Pistache::Rest::Request &request,
    [[maybe_unused]] const std::string &caller_name) const {
  return std::dynamic_pointer_cast<ParentResource>(parent_)->RequestValidate(
      request, name_);
}

void ParentResource::get(const Request &request, ResponseWriter response) {
  std::vector<Response> errors;
  if (parent_ != nullptr) {
    std::dynamic_pointer_cast<ParentResource>(parent_)->RequestValidate(request,
                                                                        name_);
  }
  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({ErrorTag::kOk, ""});
  }
  Server::ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void ParentResource::CreateReplaceUpdate(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response, bool check_mandatory) {
  std::vector<Response> errors =
      std::dynamic_pointer_cast<ParentResource>(parent_)->RequestValidate(
          request, name_);

  nlohmann::json jbody;
  if (request.body().empty()) {
    jbody = nlohmann::json::parse("{}");
  } else {
    jbody = nlohmann::json::parse(request.body());
  }

  SetDefaultIfMissing(jbody);

  auto body = BodyValidate(jbody, check_mandatory);
  errors.reserve(errors.size() + body.size());
  std::copy(std::begin(body), std::end(body), std::back_inserter(errors));
  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({ErrorTag::kCreated, ""});
  }
  Server::ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void ParentResource::post(const Request &request, ResponseWriter response) {
  CreateReplaceUpdate(request, std::move(response), true);
}

void ParentResource::put(const Request &request, ResponseWriter response) {
  CreateReplaceUpdate(request, std::move(response), true);
}

void ParentResource::patch(const Request &request, ResponseWriter response) {
  CreateReplaceUpdate(request, std::move(response), false);
}
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
