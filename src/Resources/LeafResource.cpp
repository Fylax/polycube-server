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
#include "../../include/Server/Error.h"
#include "../../include/Resources/ParentResource.h"
#include "../../include/Server/ResponseGenerator.h"
#include "../../externals/include/nlohmann/json.hpp"
#include "../../include/Server/RestServer.h"

using Pistache::Rest::Request;
using Pistache::Http::ResponseWriter;

LeafResource::LeafResource(const std::string& name, const std::string& module,
                           const std::string& rest_endpoint,
                           const std::shared_ptr<ParentResource>& parent,
                           std::unique_ptr<JsonBodyField>&& field,
                           bool configurable, bool mandatory,
                           std::unique_ptr<const std::string>&& default_value):
    Resource(name, module, rest_endpoint, parent), field_(std::move(field)),
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
  using nlohmann::detail::value_t;
  std::vector<Response> errors;
  if (body.empty()) {
    errors.push_back({ErrorTag::kMissingAttribute, name_.data()});
    return errors;
  }

  switch (body.type()) {
    case value_t::null:
      // TODO missing
    case value_t::object:
      // TODO missing
    case value_t::discarded:
      errors.push_back({ErrorTag::kBadAttribute, name_.data()});
      return errors;
    case value_t::array: {
      bool isInvalid = field_->Type() != JsonType::kEmpty &&
                       field_->Type() != JsonType::kList;
      if (isInvalid) {
        errors.push_back({ErrorTag::kBadAttribute, name_.data()});
        return errors;
      }
    }
      break;
    case value_t::string: {
      auto fieldType = field_->Type();
      bool isInvalid = fieldType != JsonType::kString &&
                       fieldType != JsonType::kInt &&
                       fieldType != JsonType::kUint &&
                       fieldType != JsonType::kDecimal;
      if (isInvalid) {
        errors.push_back({ErrorTag::kBadAttribute, name_.data()});
        return errors;
      }
    }
      break;
    case value_t::boolean: {
      bool isInvalid = field_->Type() != JsonType::kBoolean;
      if (isInvalid) {
        errors.push_back({ErrorTag::kBadAttribute, name_.data()});
        return errors;
      }
    }
      break;
    case value_t::number_integer: {
      bool isInvalid = field_->Type() != JsonType::kInt;
      if (isInvalid) {
        errors.push_back({ErrorTag::kBadAttribute, name_.data()});
        return errors;
      }
    }
      break;
    case value_t::number_unsigned: {
      bool isInvalid = field_->Type() != JsonType::kUint;
      if (isInvalid) {
        errors.push_back({ErrorTag::kBadAttribute, name_.data()});
        return errors;
      }
    }
      break;
    case value_t::number_float: {
      bool isInvalid = field_->Type() != JsonType::kDecimal;
      if (isInvalid) {
        errors.push_back({ErrorTag::kBadAttribute, name_.data()});
        return errors;
      }
    }
      break;
  }

  auto field = field_->Validate(body);
  if (field != kOk) {
    errors.push_back({field, name_.data()});
  }
  return errors;
}

std::vector<Response>
LeafResource::Validate(const Pistache::Rest::Request& value) const {
  return parent_->Validate(value);
}

bool LeafResource::IsMandatory() const {
  return mandatory_;
}

bool LeafResource::HasDefault() const {
  return default_ != nullptr;
}

void LeafResource::get(const Request& request, ResponseWriter response) {
  auto errors = parent_->Validate(request);
  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({kOk, ""});
  }
  ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void LeafResource::post(const Request& request, ResponseWriter response) {
  auto errors = Validate(request);
  std::vector<Response> body;
  if (request.body().empty()) {
    body = Validate(nlohmann::json::parse("{}"));
  } else {
    body = Validate(nlohmann::json::parse(request.body()));
  }

  errors.reserve(errors.size() + body.size());
  std::copy(std::begin(body), std::end(body),
            std::back_inserter(errors));

  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({kCreated, ""});
  }
  ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void LeafResource::put(const Request& request, ResponseWriter response) {
  post(request, std::move(response));
}
