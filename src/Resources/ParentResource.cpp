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
#include "../../include/Resources/ParentResource.h"
#include <pistache/router.h>
#include <algorithm>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <ParentResource.h>

#include "../../include/Server/Error.h"
#include "../../include/Server/ResponseGenerator.h"
#include "../../include/Server/RestServer.h"

ParentResource::ParentResource(const std::string& name,
                               const std::string& module,
                               const std::string& rest_endpoint,
                               const std::shared_ptr<ParentResource>& parent,
                               std::vector<PathParamField>&& fields,
                               bool container_presence):
    Resource(name, module, rest_endpoint, parent), fields_(std::move(fields)),
    children_(), container_presence_(container_presence) {
  using Pistache::Rest::Routes::bind;
  auto router = RestServer::Router();
  router->get(rest_endpoint_, bind(&ParentResource::get, this));
  router->post(rest_endpoint_, bind(&ParentResource::post, this));
  router->put(rest_endpoint_, bind(&ParentResource::put, this));
  router->patch(rest_endpoint_, bind(&ParentResource::patch, this));
}

ParentResource::~ParentResource() {
  using Pistache::Http::Method;
  auto router = RestServer::Router();
  router->removeRoute(Method::Get, rest_endpoint_);
  router->removeRoute(Method::Post, rest_endpoint_);
  router->removeRoute(Method::Put, rest_endpoint_);
  router->removeRoute(Method::Patch, rest_endpoint_);
}

std::vector<Response>
ParentResource::Validate(const Pistache::Rest::Request& request) const {
  std::vector<Response> errors;
  if (parent_ != nullptr) {
    errors = parent_->Validate(request);
  }

  for (const auto& field : fields_) {
    auto error = field.Validate(request);
    if (error != ErrorTag::kOk) {
      errors.push_back({error, field.Name().data()});
    }
  }
  return errors;
}

std::vector<Response>
ParentResource::Validate(const nlohmann::json& body) const {
  std::vector<Response> errors;
  for (const auto& child : children_) {
    if (body.count(child->Name()) == 0) {
      if (child->IsMandatory()) {
        errors.push_back({ErrorTag::kMissingAttribute, child->Name().data()});
      }
    } else {
      auto child_errors = child->Validate(body.at(child->Name()));
      errors.reserve(errors.size() + child_errors.size());
      std::copy(std::begin(child_errors), std::end(child_errors),
                std::back_inserter(errors));
    }
  }
  return errors;
}

void ParentResource::AddChild(std::shared_ptr<Resource> child) {
  children_.push_back(std::move(child));
}

bool ParentResource::IsMandatory() const {
  if (container_presence_) return true;
  for (const auto& child : children_) {
    if (child->IsMandatory()) return true;
  }
  return false;
}

void ParentResource::SetDefaultIfMissing(nlohmann::json& body) const {
  for (const auto& child : children_) {
    child->SetDefaultIfMissing(body[child->Name()]);
  }
}

bool ParentResource::ValidateXPath(const std::string& xpath) const {
  auto del_pos = xpath.find('/');  // current delimiter
  auto ns_pos = xpath.find(':');  // current namespace delimiter
  std::string name;
  if (ns_pos < del_pos) {  // fully-qualified name
    name = xpath.substr(ns_pos + 1, del_pos);
  } else {
    name = xpath.substr(0, del_pos);
  }
  if (name != name_) return false;

  return ValidateXPathChildren(xpath, del_pos);
}

bool ParentResource::ValidateXPathChildren(const std::string& xpath,
                                           std::size_t delimiter) const {
  auto del_pos = xpath.find('/', delimiter + 1);  // next token delimiter
  auto ns_pos = xpath.find(':', delimiter + 1);  // next namespace delimiter
  std::string ns;
  if (ns_pos < del_pos) {  // fully-qualified name
    ns = xpath.substr(del_pos + 1, ns_pos);
    // RFC 7951#6.11 states that fully-qualified names are mandatory
    // if the node is in a different module, forbidden otherwise.
    if (ns == module_) return false;
  } else {
    // sets namespace to current one to validate XPath only
    // among the children into the same namespace.
    ns = module_;
  }

  // child resources expect XPath without leading '/' character
  // and that namespace is already validated
  auto child_xpath = xpath.substr(del_pos + 1);
  for (const auto& child : children_) {
    if (child->ModuleName() == ns) {
      if (child->ValidateXPath(child_xpath)) return true;
    }
  }
  return false;
}

void ParentResource::get(const Request& request, ResponseWriter response) {
  std::vector<Response> errors;
  if (parent_ != nullptr) errors = parent_->Validate(request);
  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({ErrorTag::kOk, ""});
  }
  ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void ParentResource::CreateOrReplace(const Request& request,
                                     ResponseWriter response) {
  std::vector<Response> errors;
  if (parent_ != nullptr) errors = parent_->Validate(request);

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
    errors.push_back({ErrorTag::kCreated, ""});
  }
  ResponseGenerator::Generate(std::move(errors), std::move(response));
}

void ParentResource::post(const Request& request, ResponseWriter response) {
  CreateOrReplace(request, std::move(response));
}

void ParentResource::put(const Request& request, ResponseWriter response) {
  CreateOrReplace(request, std::move(response));
}

void ParentResource::patch(const Request& request, ResponseWriter response) {
  std::vector<Response> errors;
  if (parent_ != nullptr) errors = parent_->Validate(request);
  auto body = nlohmann::json::parse(request.body());

  for (const auto& child : children_) {
    if (body.count(child->Name()) != 0) {
      auto child_errors = child->Validate(body.at(child->Name()));
      errors.reserve(errors.size() + child_errors.size());
      std::copy(std::begin(child_errors), std::end(child_errors),
                std::back_inserter(errors));
    }
  }
  // TODO: call user code and merge responses
  if (errors.empty()) {
    errors.push_back({ErrorTag::kOk, ""});
  }
  ResponseGenerator::Generate(std::move(errors), std::move(response));
}

