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
#include "../../../../include/Resources/Data/Lib/ConcreteFactory.h"

#include <dlfcn.h>

#include <memory>
#include <string>
#include <vector>

#include "../../../../include/Resources/Endpoint/ParentResource.h"
#include "../../../../include/Resources/Endpoint/LeafResource.h"
#include "../../../../include/Resources/Endpoint/ListResource.h"

#include "../../../../include/Resources/Data/Lib/EntryPoint.h"
#include "../../../../include/Resources/Data/Lib/ParentResource.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {
using EntryPoint::GenerateName;
using EntryPoint::Operation;
ConcreteFactory::ConcreteFactory(const std::string &file_name)
    : handle_(::dlopen(file_name.data(), RTLD_LAZY),
              [](void *h) { ::dlclose(h); }) {
  if (!handle_) {
    throw std::logic_error("Cannot load service implementation " + file_name);
  }
}

const std::string ConcreteFactory::Yang() const {
  return std::invoke(LoadHandler<const char *(void)>("data_model"));
}

template <typename T>
std::function<T> ConcreteFactory::LoadHandler(
    const std::string &function_name) const {
  void *result = ::dlsym(handle_.get(), function_name.data());
  if (!result) {
    char *const error = ::dlerror();
    if (error) {
      throw std::domain_error("Cannot find handler " + function_name);
    }
  }
  return reinterpret_cast<T *>(result);
}

std::unique_ptr<Endpoint::CaseResource> ConcreteFactory::RestCase(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module,
    std::shared_ptr<Endpoint::ParentResource> parent) const {
  throw std::domain_error(
      "Yang case node not supported with shared object protocol.");
}

std::unique_ptr<Endpoint::ChoiceResource> ConcreteFactory::RestChoice(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, std::shared_ptr<Endpoint::ParentResource> parent,
    bool mandatory, std::unique_ptr<const std::string> &&default_case) const {
  throw std::domain_error(
      "Yang choice node not supported with shared object protocol.");
}

std::unique_ptr<Endpoint::LeafResource> ConcreteFactory::RestLeaf(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, std::string rest_endpoint,
    std::shared_ptr<Endpoint::ParentResource> parent,
    std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
    bool mandatory, std::unique_ptr<const std::string> &&default_value) const {
  return nullptr;
}

std::unique_ptr<Endpoint::LeafListResource> ConcreteFactory::RestLeafList(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, std::string rest_endpoint,
    std::shared_ptr<Endpoint::ParentResource> parent,
    std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
    bool mandatory, std::vector<std::string> &&default_value) const {
  throw std::domain_error(
      "Yang case leaf-list not supported with shared object protocol.");
}

std::unique_ptr<Endpoint::ListResource> ConcreteFactory::RestList(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, std::string rest_endpoint,
    std::string rest_endpoint_whole_list,
    std::shared_ptr<Endpoint::ParentResource> parent,
    std::vector<Resources::Body::ListKey> &&keys) const {
  return nullptr;
}

std::unique_ptr<Endpoint::ParentResource> ConcreteFactory::RestGeneric(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, std::string rest_endpoint,
    std::shared_ptr<Endpoint::ParentResource> parent,
    bool container_presence) const {
  auto create_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          GenerateName(tree_names, Operation::kCreate));
  auto replace_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          GenerateName(tree_names, Operation::kReplace));
  auto update_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          GenerateName(tree_names, Operation::kUpdate));
  auto read_handler = LoadHandler<Response(const char *, Key *, size_t)>(
      GenerateName(tree_names, Operation::kRead));
  auto delete_handler = LoadHandler<Response(const char *, Key *, size_t)>(
      GenerateName(tree_names, Operation::kDelete));
  return std::make_unique<ParentResource>(
      std::move(create_handler), std::move(replace_handler),
      std::move(update_handler), std::move(read_handler),
      std::move(delete_handler), std::move(name), std::move(module),
      std::move(rest_endpoint), std::move(parent), container_presence);
}

}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
