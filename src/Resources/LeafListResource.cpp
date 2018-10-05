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
#include <LeafListResource.h>

#include "LeafListResource.h"
#include <algorithm>
#include <memory>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

#include "../../include/Types/Dummies.h"
#include "../../include/Resources/ParentResource.h"
#include "../../include/Server/ResponseGenerator.h"
#include "../../include/Server/RestServer.h"

LeafListResource::LeafListResource(std::string name, std::string module,
                                   std::string rest_endpoint,
                                   std::shared_ptr<ParentResource> parent,
                                   std::unique_ptr<JsonBodyField>&& field,
                                   bool configurable, bool mandatory,
                                   std::vector<std::string>&& default_value):
    LeafResource(std::move(name), std::move(module), std::move(rest_endpoint),
                 std::move(parent), std::move(field), configurable, mandatory,
                 nullptr), default_(std::move(default_value)) {
  using Pistache::Rest::Routes::bind;
  auto router = RestServer::Router();
  router->get(rest_endpoint_ + "/:entry",
              bind(&LeafListResource::get_entry, this));
}

LeafListResource::~LeafListResource() {
  using Pistache::Http::Method;
  auto router = RestServer::Router();
  router->removeRoute(Method::Get, rest_endpoint_ + "/:entry");
}

std::vector<Response>
LeafListResource::Validate(const nlohmann::json& body) const {
  std::vector<Response> errors;
  if (body.type() != nlohmann::detail::value_t::array) {
    errors.push_back({ErrorTag::kBadAttribute, name_.data()});
    return errors;
  }

  for (const auto& element : body) {
    const auto& inner_errors = LeafResource::Validate(element);
    errors.reserve(errors.size() + inner_errors.size());
    std::copy(std::begin(inner_errors), std::end(inner_errors),
              std::back_inserter(errors));
  }
  return errors;
}

void LeafListResource::SetDefaultIfMissing(nlohmann::json& body) const {
  if (body.empty() && !default_.empty()) {
    for (const auto& element : default_) {
      body.push_back(element);
    }
  }
}

void
LeafListResource::get_entry(const Request& request, ResponseWriter response) {
  auto errors = parent_->Validate(request, name_);
  auto value = nlohmann::json::parse(request.param(":entry").as<std::string>());
  auto body = LeafResource::Validate(value);
  errors.reserve(errors.size() + body.size());
  std::copy(std::begin(body), std::end(body),
            std::back_inserter(errors));
  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({kOk, ""});
  }
  ResponseGenerator::Generate(std::move(errors), std::move(response));
}
