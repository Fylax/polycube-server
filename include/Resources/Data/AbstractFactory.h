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
#include <queue>
#include <string>

#include "../Body/AbstractFactory.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

#include "../../../externals/include/nlohmann/json.hpp"

#pragma GCC diagnostic pop

namespace polycube::polycubed::Rest::Resources::Endpoint {
class CaseResource;

class ChoiceResource;

class LeafResource;

class LeafListResource;

class ListResource;

class ParentResource;

class Service;
} // namespace polycube::polycubed::Rest::Resources::Endpoint

namespace polycube::polycubed::Rest::Resources::Data {
enum class Protocol {
  kLib
};
class AbstractFactory: public Body::AbstractFactory {
 public :
  static std::unique_ptr<AbstractFactory>
  Concrete(const nlohmann::json& request_body);

  virtual const std::string Yang() const = 0;

  virtual std::unique_ptr<Endpoint::CaseResource>
  RestCase(const std::queue<std::string>& tree_names, std::string name,
           std::string module,
           std::shared_ptr<Endpoint::ParentResource> parent) const = 0;

  virtual std::unique_ptr<Endpoint::ChoiceResource>
  RestChoice(const std::queue<std::string>& tree_names, std::string name,
             std::string module,
             std::shared_ptr<Endpoint::ParentResource> parent, bool mandatory,
             std::unique_ptr<const std::string>&& default_case) const = 0;

  virtual std::unique_ptr<Endpoint::LeafResource>
  RestLeaf(const std::queue<std::string>& tree_names, std::string name,
           std::string module, std::string rest_endpoint,
           std::shared_ptr<Endpoint::ParentResource> parent,
           std::unique_ptr<Body::JsonBodyField>&& field, bool configuration,
           bool mandatory,
           std::unique_ptr<const std::string>&& default_value) const = 0;

  virtual std::unique_ptr<Endpoint::LeafListResource>
  RestLeafList(const std::queue<std::string>& tree_names, std::string name,
               std::string module, std::string rest_endpoint,
               std::shared_ptr<Endpoint::ParentResource> parent,
               std::unique_ptr<Body::JsonBodyField>&& field, bool configuration,
               bool mandatory,
               std::vector<std::string>&& default_value) const = 0;

  virtual std::unique_ptr<Endpoint::ListResource>
  RestList(const std::queue<std::string>& tree_names, std::string name,
           std::string module, std::string rest_endpoint,
           std::string rest_endpoint_whole_list,
           std::shared_ptr<Endpoint::ParentResource> parent,
           std::vector<Resources::Body::ListKey>&& keys) const = 0;

  virtual std::unique_ptr<Endpoint::ParentResource>
  RestGeneric(const std::queue<std::string>& tree_names, std::string name,
              std::string module, std::string rest_endpoint,
              std::shared_ptr<Endpoint::ParentResource> parent,
              bool container_presence) const = 0;

  virtual std::unique_ptr<Endpoint::Service>
  RestService(const std::queue<std::string>& tree_names, std::string name,
              std::string base_endpoint) const = 0;

  static void RemoveService(const std::string &name);
protected:
  static void AddService(std::string name, Protocol protocol);
private:
  static std::unordered_map<std::string, Protocol> services_;
};
} // namespace polycube::polycubed::Rest::Resources::Data