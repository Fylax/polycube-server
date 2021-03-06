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
#include <Lib/ConcreteFactory.h>


#include "../../../../include/Resources/Endpoint/LeafResource.h"
#include "../../../../include/Resources/Endpoint/ListResource.h"
#include "../../../../include/Resources/Endpoint/ParentResource.h"
#include "../../../../include/Resources/Endpoint/Service.h"

#include "../../../../include/Resources/Data/Lib/EntryPoint.h"
#include "../../../../include/Resources/Data/Lib/LeafResource.h"
#include "../../../../include/Resources/Data/Lib/ListResource.h"
#include "../../../../include/Resources/Data/Lib/ParentResource.h"
#include "../../../../include/Resources/Data/Lib/Service.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {
using EntryPoint::GenerateName;
using EntryPoint::Operation;

std::unordered_map<std::string, std::shared_ptr<void>> ConcreteFactory::handlers_ = {};

ConcreteFactory::ConcreteFactory(const std::string &file_name)
    : handle_{::dlopen(file_name.data(), RTLD_NOW),
              [](void *h) { if(h != nullptr) ::dlclose(h); }} {
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
      throw std::invalid_argument(std::string{error});
    }
  }
  return reinterpret_cast<T *>(result);
}

std::unique_ptr<Endpoint::CaseResource> ConcreteFactory::RestCase(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module,
    const Body::ParentResource * const parent) const {
  throw std::invalid_argument(
      "Yang case node not supported with shared object protocol.");
}

std::unique_ptr<Endpoint::ChoiceResource> ConcreteFactory::RestChoice(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, const Body::ParentResource * const parent,
    bool mandatory, std::unique_ptr<const std::string> &&default_case) const {
  throw std::invalid_argument(
      "Yang choice node not supported with shared object protocol.");
}

std::unique_ptr<Endpoint::LeafResource> ConcreteFactory::RestLeaf(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, std::string rest_endpoint,
    const Body::ParentResource * const parent,
    std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
    bool mandatory, std::unique_ptr<const std::string> &&default_value) const {
  auto read_handler = LoadHandler<Response(const char *, Key *, size_t)>(
      GenerateName(tree_names, Operation::kRead));
  if (!configuration) {
    return std::make_unique<LeafResource>(
        std::move(read_handler), std::move(name), std::move(module),
        std::move(rest_endpoint), parent, std::move(field),
        mandatory, std::move(default_value));
  }
  auto replace_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          GenerateName(tree_names, Operation::kReplace));
  return std::make_unique<LeafResource>(
      std::move(replace_handler), std::move(read_handler), std::move(name),
      std::move(module), std::move(rest_endpoint), parent,
      std::move(field), mandatory, std::move(default_value));
}

std::unique_ptr<Endpoint::LeafListResource> ConcreteFactory::RestLeafList(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, std::string rest_endpoint,
    const Body::ParentResource * const parent,
    std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
    bool mandatory, std::vector<std::string> &&default_value) const {
  throw std::invalid_argument(
      "Yang case leaf-list not supported with shared object protocol.");
}

std::unique_ptr<Endpoint::ListResource> ConcreteFactory::RestList(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, std::string rest_endpoint,
    std::string rest_endpoint_whole_list,
    const Body::ParentResource * const parent,
    std::vector<Resources::Body::ListKey> &&keys) const {
  auto create_name = GenerateName(tree_names, Operation::kCreate);
  auto replace_name = GenerateName(tree_names, Operation::kReplace);
  auto update_name = GenerateName(tree_names, Operation::kUpdate);
  auto read_name = GenerateName(tree_names, Operation::kRead);
  auto delete_name = GenerateName(tree_names, Operation::kDelete);

  auto create_entry_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          create_name);
  auto replace_entry_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          replace_name);
  auto update_entry_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          update_name);
  auto read_entry_handler =
      LoadHandler<Response(const char *, Key *, size_t)>(read_name);
  auto delete_entry_handler =
      LoadHandler<Response(const char *, Key *, size_t)>(delete_name);

  auto sname = name;
  auto length = sname.length();
  std::replace(std::begin(sname), std::end(sname), '-', '_');
  std::replace(std::begin(sname), std::end(sname), '.', '_');
  std::string replace = sname + "_list";
  create_name.replace(create_name.find(sname), length, replace);
  replace_name.replace(replace_name.find(sname), length, replace);
  update_name.replace(update_name.find(sname), length, replace);
  read_name.replace(read_name.find(sname), length, replace);
  delete_name.replace(delete_name.find(sname), length, replace);

  auto create_whole_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          create_name);
  auto replace_whole_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          replace_name);
  auto update_whole_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          update_name);
  auto read_whole_handler =
      LoadHandler<Response(const char *, Key *, size_t)>(read_name);
  auto delete_whole_handler =
      LoadHandler<Response(const char *, Key *, size_t)>(delete_name);

  auto help_handler = LoadHandler<Response(const char *, Key *, size_t)>(
      read_name + "_get_list");

  return std::make_unique<ListResource>(
      std::move(create_entry_handler), std::move(replace_entry_handler),
      std::move(update_entry_handler), std::move(read_entry_handler),
      std::move(delete_entry_handler), std::move(create_whole_handler),
      std::move(replace_whole_handler), std::move(update_whole_handler),
      std::move(read_whole_handler), std::move(delete_whole_handler),
      std::move(help_handler), std::move(name), std::move(module),
      std::move(rest_endpoint), std::move(rest_endpoint_whole_list),
      parent, std::move(keys));
}

std::unique_ptr<Body::ParentResource> ConcreteFactory::RestGeneric(
    const std::queue<std::string> &tree_names, std::string name,
    std::string module, std::string rest_endpoint,
    const Body::ParentResource * const parent,
    bool container_presence, bool rpc_action) const {
  auto create_handler =
      LoadHandler<Response(const char *, Key *, size_t, const char *)>(
          GenerateName(tree_names, Operation::kCreate));
  if (rpc_action) {
    return std::make_unique<ParentResource>(
        std::move(create_handler), std::move(name), std::move(module),
        std::move(rest_endpoint), parent);
  }
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
      std::move(rest_endpoint), parent, container_presence);
}

std::unique_ptr<Endpoint::Service> ConcreteFactory::RestService(
    [[maybe_unused]] const std::queue<std::string> &tree_names,
    std::string name, std::string base_endpoint) const {
  // Required for ensuring handle lifetime
  handlers_[name] = handle_;

  std::string create_single{"create_" + name + "_by_id"};
  std::string update_single{"create_" + name + "_by_id"};
  std::string read_single{"create_" + name + "_by_id"};
  std::string replace_single{"create_" + name + "_by_id"};
  std::string delete_single{"create_" + name + "_by_id"};

  std::string read_whole{"read_" + name + "_list_by_id"};
  std::string update_whole{"update_" + name + "_list_by_id"};
  std::string help{read_whole + "_get_list"};

  auto create_handler =
      LoadHandler<Response(const char *, const char *)>(create_single);
  auto update_handler =
      LoadHandler<Response(const char *, const char *)>(update_single);
  auto read_handler = LoadHandler<Response(const char *)>(read_single);
  auto replace_handler =
      LoadHandler<Response(const char *, const char *)>(replace_single);
  auto delete_handler = LoadHandler<Response(const char *)>(delete_single);

  auto read_list_handler = LoadHandler<Response(void)>(read_whole);
  auto help_handler = LoadHandler<Response(void)>(help);
  auto update_list_handler = LoadHandler<Response(const char *)>(update_whole);

  return std::make_unique<Service>(
      std::move(create_handler), std::move(update_handler),
      std::move(read_handler), std::move(replace_handler),
      std::move(delete_handler), std::move(read_list_handler),
      std::move(help_handler), std::move(update_list_handler), std::move(name),
      std::move(base_endpoint));
}

void ConcreteFactory::UnregisterService(const std::string& name) {
handlers_.erase(name);
}

}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
