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
#include <vector>

#include "../AbstractFactory.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {

class ConcreteFactory : public Data::AbstractFactory {
 public:
  explicit ConcreteFactory(const std::string &file_name);

  ~ConcreteFactory() final = default;

  const std::string Yang() const final;

  std::unique_ptr<Endpoint::CaseResource> RestCase(
      const std::queue<std::string> &tree_names, std::string name,
      std::string module,
      const Body::ParentResource * parent) const final;

  std::unique_ptr<Endpoint::ChoiceResource> RestChoice(
      const std::queue<std::string> &tree_names, std::string name,
      std::string module, const Body::ParentResource * parent,
      bool mandatory,
      std::unique_ptr<const std::string> &&default_case) const final;

  std::unique_ptr<Endpoint::LeafResource> RestLeaf(
      const std::queue<std::string> &tree_names, std::string name,
      std::string module, std::string rest_endpoint,
      const Body::ParentResource * parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory,
      std::unique_ptr<const std::string> &&default_value) const final;

  std::unique_ptr<Endpoint::LeafListResource> RestLeafList(
      const std::queue<std::string> &tree_names, std::string name,
      std::string module, std::string rest_endpoint,
      const Body::ParentResource * parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory, std::vector<std::string> &&default_value) const final;

  std::unique_ptr<Endpoint::ListResource> RestList(
      const std::queue<std::string> &tree_names, std::string name,
      std::string module, std::string rest_endpoint,
      std::string rest_endpoint_whole_list,
      const Body::ParentResource * parent,
      std::vector<Resources::Body::ListKey> &&keys) const final;

  std::unique_ptr<Body::ParentResource> RestGeneric(
      const std::queue<std::string> &tree_names, std::string name,
      std::string module, std::string rest_endpoint,
      const Body::ParentResource * parent,
      bool container_presence, bool rpc_action) const final;

  std::unique_ptr<Endpoint::Service> RestService(
      const std::queue<std::string> &tree_names, std::string name,
      std::string base_endpoint) const final;

  template <typename T>
  std::function<T> LoadHandler(const std::string &function_name) const;

  static void UnregisterService(const std::string &name);

 private:
  static std::unordered_map<std::string, std::shared_ptr<void>> handlers_;

  std::shared_ptr<void> handle_;
};
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
