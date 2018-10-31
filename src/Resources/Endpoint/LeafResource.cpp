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
#include "../../../include/Resources/Endpoint/LeafResource.h"

#include <LeafResource.h>
#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include "../../../include/Resources/Body/ListKey.h"

#include "../../../include/Server/ResponseGenerator.h"
#include "../../../include/Server/RestServer.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
using ListKeyValues = std::vector<std::pair<Body::ListKey, std::string>>;
using PerListKeyValues = std::stack<ListKeyValues>;

LeafResource::LeafResource(std::string rest_endpoint)
    : LeafResource("", "", std::move(rest_endpoint), nullptr, nullptr, false,
                   false, nullptr) {}

LeafResource::LeafResource(
    std::string name, std::string module, std::string rest_endpoint,
    std::shared_ptr<ParentResource> parent,
    std::unique_ptr<polycube::polycubed::Rest::Resources::Body::JsonBodyField>
        &&field,
    bool configurable, bool mandatory,
    std::unique_ptr<const std::string> &&default_value)
    : Body::LeafResource(std::move(name), std::move(module), std::move(parent),
                         std::move(field), configurable, mandatory,
                         std::move(default_value)),
      Endpoint::Resource(std::move(rest_endpoint)) {
  using Pistache::Rest::Routes::bind;
  auto router = Server::RestServer::Router();
  router->get(rest_endpoint_, bind(&LeafResource::get, this));
  if (configuration_) {
    router->put(rest_endpoint_, bind(&LeafResource::put, this));
  }
}

LeafResource::~LeafResource() {
  using Pistache::Http::Method;
  auto router = Server::RestServer::Router();
  router->removeRoute(Method::Get, rest_endpoint_);
  if (configuration_) {
    router->removeRoute(Method::Put, rest_endpoint_);
  }
}

void LeafResource::Keys(const Pistache::Rest::Request &request,
                        PerListKeyValues &parsed) const {
  return std::dynamic_pointer_cast<ParentResource>(parent_)->Keys(request,
                                                                  parsed);
}

void LeafResource::get(const Request &request, ResponseWriter response) {
  auto errors =
      std::dynamic_pointer_cast<ParentResource>(parent_)->RequestValidate(
          request, name_);
  if (errors.empty()) {
    const auto &cube_name = Service::Cube(request);
    PerListKeyValues keys{};
    std::dynamic_pointer_cast<ParentResource>(parent_)->Keys(request, keys);
    errors.push_back(ReadValue(cube_name, keys));
  }
  Server::ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void LeafResource::CreateReplaceUpdate(const Pistache::Rest::Request &request,
                                       Pistache::Http::ResponseWriter response,
                                       bool update, bool check_mandatory) {
  auto errors = RequestValidate(request, name_);
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

  if (errors.empty()) {
    auto op = OperationType(update, check_mandatory);
    const auto cube_name = Service::Cube(request);
    PerListKeyValues keys{};
    std::dynamic_pointer_cast<ParentResource>(parent_)->Keys(request, keys);
    auto resp = WriteValue(cube_name, jbody, keys, op);
    if (resp.error_tag == ErrorTag::kOk) {
      errors.push_back({ErrorTag::kCreated, ""});
    } else {
      errors.push_back(resp);
    }
  }
  Server::ResponseGenerator::Generate(std::move(errors), std::move(response));
}

std::vector<Response> LeafResource::RequestValidate(
    const Pistache::Rest::Request &request,
    [[maybe_unused]] const std::string &caller_name) const {
  return std::dynamic_pointer_cast<ParentResource>(parent_)->RequestValidate(
      request, name_);
}

void LeafResource::put(const Request &request, ResponseWriter response) {
  CreateReplaceUpdate(request, std::move(response), true, true);
}
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
