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
#include "../../../include/Resources/Endpoint/ListResource.h"

#include <ListResource.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../../include/Resources/Body/ListKey.h"
#include "../../../include/Resources/Endpoint/PathParamField.h"
#include "../../../include/Server/ResponseGenerator.h"
#include "../../../include/Server/RestServer.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
ListResource::ListResource(std::string name, std::string module,
                           std::shared_ptr<ParentResource> parent,
                           std::string rest_endpoint,
                           std::string rest_endpoint_multiple,
                           std::vector<Body::ListKey> &&keys,
                           bool configuration)
    : Body::ParentResource(std::move(name), std::move(module),
                           std::move(parent), false),
      ParentResource(std::move(rest_endpoint), configuration, false),
      Body::ListResource(std::move(keys)),
      key_params_{},
      multiple_endpoint_(std::move(rest_endpoint_multiple)) {
  for (const auto &key : keys_) {
    key_params_.emplace_back(key.Name(), key.Validators());
  }
  using Pistache::Rest::Routes::bind;
  auto router = Server::RestServer::Router();
  router->get(multiple_endpoint_, bind(&ListResource::get_multiple, this));
  if (configuration_) {
    router->post(multiple_endpoint_, bind(&ListResource::post_multiple, this));
    router->put(multiple_endpoint_, bind(&ListResource::put_multiple, this));
    router->patch(multiple_endpoint_,
                  bind(&ListResource::patch_multiple, this));
    router->del(multiple_endpoint_, bind(&ListResource::del_multiple, this));
  }
}

ListResource::~ListResource() {
  using Pistache::Http::Method;
  auto router = Server::RestServer::Router();
  router->removeRoute(Method::Get, multiple_endpoint_);
  if (configuration_) {
    router->removeRoute(Method::Post, multiple_endpoint_);
    router->removeRoute(Method::Put, multiple_endpoint_);
    router->removeRoute(Method::Patch, multiple_endpoint_);
    router->removeRoute(Method::Delete, multiple_endpoint_);
  }
}

std::vector<Response> ListResource::RequestValidate(
    const Pistache::Rest::Request &request,
    const std::string &caller_name) const {
  auto errors = ParentResource::RequestValidate(request, caller_name);
  for (const auto &key_param : key_params_) {
    auto error = key_param.Validate(request);
    if (error != ErrorTag::kOk) {
      errors.push_back({error, key_param.Name().data()});
    }
  }
  return errors;
}

void ListResource::Keys(const Pistache::Rest::Request &request,
                        PerListKeyValues &parsed) const {
  ListKeyValues keys{};
  for (const auto &k : keys_) {
    keys.emplace_back(k, request.param(k.Name()).as<std::string>());
  }
  parsed.push(std::move(keys));
  std::dynamic_pointer_cast<ParentResource>(parent_)->Keys(request, parsed);
}

ListResource::ListResource(std::string rest_endpoint,
                           std::string rest_endpoint_multiple,
                           std::vector<Body::ListKey> &&keys,
                           bool configuration)
    : ListResource("", "", nullptr, std::move(rest_endpoint),
                   std::move(rest_endpoint_multiple), std::move(keys),
                   configuration) {}

void ListResource::CreateReplaceUpdateWhole(
    const Pistache::Rest::Request &request, ResponseWriter response,
    bool update, bool check_mandatory) {
  std::vector<Response> errors =
      std::dynamic_pointer_cast<ParentResource>(parent_)->RequestValidate(
          request, name_);

  nlohmann::json jbody;
  if (request.body().empty()) {
    jbody = nlohmann::json::parse("[]");
  } else {
    jbody = nlohmann::json::parse(request.body());
  }

  if (jbody.type() != nlohmann::detail::value_t::array) {
    Server::ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kInvalidValue, ""}},
        std::move(response));
    return;
  }

  for (auto elem : jbody) {
    SetDefaultIfMissing(elem);

    auto body = BodyValidate(elem, check_mandatory);
    errors.reserve(errors.size() + body.size());
    std::copy(std::begin(body), std::end(body), std::back_inserter(errors));
  }
  if (errors.empty()) {
    auto op = OperationType(update, check_mandatory);
    const auto cube_name = Service::Cube(request);
    PerListKeyValues keys{};
    std::dynamic_pointer_cast<ParentResource>(parent_)->Keys(request, keys);
    auto resp = WriteWhole(cube_name, jbody, keys, op);
    if (resp.error_tag == ErrorTag::kOk) {
      errors.push_back({ErrorTag::kCreated, ""});
    } else {
      errors.push_back(resp);
    }
  }
  Server::ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void ListResource::get_multiple(const Request &request,
                                ResponseWriter response) {
  std::vector<Response> errors;
  if (parent_ != nullptr) {
    std::dynamic_pointer_cast<ParentResource>(parent_)->RequestValidate(request,
                                                                        name_);
  }
  if (errors.empty()) {
    const auto &cube_name = Service::Cube(request);
    PerListKeyValues keys{};
    std::dynamic_pointer_cast<ParentResource>(parent_)->Keys(request, keys);
    errors.push_back(ReadWhole(cube_name, keys));
  }
  Server::ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void ListResource::post_multiple(const Request &request,
                                 ResponseWriter response) {
  CreateReplaceUpdateWhole(request, std::move(response), false, true);
}

void ListResource::put_multiple(const Request &request,
                                ResponseWriter response) {
  CreateReplaceUpdateWhole(request, std::move(response), true, true);
}

void ListResource::patch_multiple(const Request &request,
                                  ResponseWriter response) {
  CreateReplaceUpdateWhole(request, std::move(response), true, false);
}

void ListResource::del_multiple(const Request &request,
                                   ResponseWriter response) {
  std::vector<Response> errors;
  if (parent_ != nullptr) {
    std::dynamic_pointer_cast<ParentResource>(parent_)->RequestValidate(request,
                                                                        name_);
  }
  if (errors.empty()) {
    const auto &cube_name = Service::Cube(request);
    PerListKeyValues keys{};
    std::dynamic_pointer_cast<ParentResource>(parent_)->Keys(request, keys);
    errors.push_back(DeleteWhole(cube_name, keys));
  }
  Server::ResponseGenerator::Generate(std::move(errors), std::move(response));
}
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
