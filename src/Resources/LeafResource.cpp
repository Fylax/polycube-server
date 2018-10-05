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

#include "../../include/Resources/LeafResource.h"
#include <pistache/router.h>
#include <algorithm>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <LeafResource.h>

#include "../../include/Server/Error.h"
#include "../../include/Resources/ParentResource.h"
#include "../../include/Server/ResponseGenerator.h"
#include "../../externals/include/nlohmann/json.hpp"
#include "../../include/Server/RestServer.h"

using Pistache::Rest::Request;
using Pistache::Http::ResponseWriter;

LeafResource::LeafResource(std::string name, std::string module,
                           std::string rest_endpoint,
                           std::shared_ptr<ParentResource> parent,
                           std::unique_ptr<JsonBodyField>&& field,
                           bool configurable, bool mandatory,
                           std::unique_ptr<const std::string>&& default_value):
    Resource(std::move(name), std::move(module), std::move(rest_endpoint),
             std::move(parent)), field_(std::move(field)),
    configurable_(configurable), mandatory_(mandatory),
    default_(std::move(default_value)) {
  using Pistache::Rest::Routes::bind;
  auto router = RestServer::Router();
  router->get(rest_endpoint_, bind(&LeafResource::get, this));
  if (configurable_) {
    router->post(rest_endpoint_, bind(&LeafResource::post, this));
    router->put(rest_endpoint_, bind(&LeafResource::put, this));
  }
}

LeafResource::~LeafResource() {
  using Pistache::Http::Method;
  auto router = RestServer::Router();
  router->removeRoute(Method::Get, rest_endpoint_);
  if (configurable_) {
    router->removeRoute(Method::Post, rest_endpoint_);
    router->removeRoute(Method::Put, rest_endpoint_);
  }
}

std::vector<Response>
LeafResource::Validate(const nlohmann::json& body) const {
  std::vector<Response> errors;
  if (body.empty()) {
    errors.push_back({ErrorTag::kMissingAttribute, name_.data()});
    return errors;
  }

  auto field = field_->Validate(body);
  if (field != kOk) {
    errors.push_back({field, name_.data()});
  }
  return errors;
}

std::vector<Response>
LeafResource::Validate(const Pistache::Rest::Request& value,
                       const std::string& caller_name) const {
  return parent_->Validate(value, name_);
}

bool LeafResource::IsMandatory() const {
  return mandatory_;
}

void LeafResource::SetDefaultIfMissing(nlohmann::json& body) const {
  if (body.empty() && default_ != nullptr) body = *default_;
}

bool LeafResource::ValidateXPath(const std::string& xpath) const {
  auto ns_pos = xpath.find(':');  // current namespace delimiter
  std::string name;
  if (ns_pos != std::string::npos) {  // fully-qualified name
    name = xpath.substr(ns_pos + 1);
  } else {
    name = xpath;
  }
  return name == name_;
}

void LeafResource::get(const Request& request, ResponseWriter response) {
  auto errors = parent_->Validate(request, name_);
  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({kOk, ""});
  }
  ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void
LeafResource::CreateOrReplace(const Request& request, ResponseWriter response) {
  auto errors = Validate(request, name_);
  nlohmann::json jbody;
  if (request.body().empty()) {
    jbody = nlohmann::json::parse("{}");
  } else {
    jbody = nlohmann::json::parse(request.body());
  }

  SetDefaultIfMissing(jbody);

  auto body = Validate(jbody);
  errors.reserve(errors.size() + body.size());
  std::copy(std::begin(body), std::end(body),
            std::back_inserter(errors));

  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({kCreated, ""});
  }
  ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void LeafResource::post(const Request& request, ResponseWriter response) {
  CreateOrReplace(request, std::move(response));
}

void LeafResource::put(const Request& request, ResponseWriter response) {
  CreateOrReplace(request, std::move(response));
}
