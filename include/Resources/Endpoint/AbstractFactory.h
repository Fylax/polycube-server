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
#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../Body/AbstractFactory.h"

namespace polycube::polycubed::Rest::Resources::Body {
class ListKey;
}

namespace polycube::polycubed::Rest::Resources::Endpoint {
class CaseResource;
class ChoiceResource;
class LeafResource;
class LeafListResource;
class ListResource;
class ParentResource;
class Service;

class AbstractFactory : public Body::AbstractFactory {
 public:
  virtual std::unique_ptr<CaseResource> RestCase(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent) const = 0;

  virtual std::unique_ptr<ChoiceResource> RestChoice(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent, bool mandatory,
      std::unique_ptr<const std::string> &&default_case) const = 0;

  virtual std::unique_ptr<LeafResource> RestLeaf(
      std::string name, std::string module, std::string rest_endpoint,
      std::shared_ptr<ParentResource> parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory,
      std::unique_ptr<const std::string> &&default_value) const = 0;

  virtual std::unique_ptr<LeafListResource> RestLeafList(
      std::string name, std::string module, std::string rest_endpoint,
      std::shared_ptr<ParentResource> parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory, std::vector<std::string> &&default_value) const = 0;

  virtual std::unique_ptr<ListResource> RestList(
      std::string name, std::string module, std::string rest_endpoint,
      std::string rest_endpoint_whole_list,
      std::shared_ptr<ParentResource> parent,
      std::vector<Resources::Body::ListKey> &&keys) const = 0;

  virtual std::unique_ptr<ParentResource> RestGeneric(
      std::string name, std::string module, std::string rest_endpoint,
      std::shared_ptr<ParentResource> parent,
      bool container_presence) const = 0;

  virtual std::unique_ptr<Service> RestService(
      std::string name, std::string base_endpoint) const = 0;
};
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
