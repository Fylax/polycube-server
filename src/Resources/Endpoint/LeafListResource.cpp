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
#include "../../../include/Resources/Endpoint/LeafListResource.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../../include/Resources/Body/JsonBodyField.h"
#include "../../../include/Resources/Body/LeafResource.h"
#include "../../../include/Server/ResponseGenerator.h"
#include "../../../include/Server/RestServer.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
LeafListResource::LeafListResource(std::string name, std::string module,
                                   std::string rest_endpoint,
                                   std::shared_ptr<ParentResource> parent,
                                   std::unique_ptr<Body::JsonBodyField> &&field,
                                   bool configurable, bool mandatory,
                                   std::vector<std::string> &&default_value)
    : Body::LeafResource(std::move(name), std::move(module), std::move(parent),
                         std::move(field), configurable, mandatory, nullptr),
      LeafResource(std::move(rest_endpoint)),
      Body::LeafListResource(std::move(default_value)) {
  using Pistache::Rest::Routes::bind;
  auto router = Server::RestServer::Router();
  router->get(rest_endpoint_ + "/:entry",
              bind(&LeafListResource::get_entry, this));
}

LeafListResource::~LeafListResource() {
  using Pistache::Http::Method;
  auto router = Server::RestServer::Router();
  router->removeRoute(Method::Get, rest_endpoint_ + "/:entry");
}

void LeafListResource::get_entry(const Request &request,
                                 ResponseWriter response) {
  auto errors =
      std::dynamic_pointer_cast<ParentResource>(parent_)->RequestValidate(
          request, name_);
  auto value = nlohmann::json::parse(request.param(":entry").as<std::string>());
  auto body = LeafResource::BodyValidate(value, false);
  errors.reserve(errors.size() + body.size());
  std::copy(std::begin(body), std::end(body), std::back_inserter(errors));
  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({kOk, ""});
  }
  Server::ResponseGenerator::Generate(std::move(errors), std::move(response));
}
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
