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
#include "../../../../include/Resources/Data/Lib/LeafResource.h"

#include <functional>
#include <memory>
#include <string>
#include <utility>

#include "../../../../include/Resources/Data/Lib/KeyListArray.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {
NonConfigLeafResource::NonConfigLeafResource(
    std::function<Response(const char *, Key *, size_t)> read_handler,
    std::string name, std::string module, std::string rest_endpoint,
    std::shared_ptr<Endpoint::ParentResource> parent,
    std::unique_ptr<Body::JsonBodyField> &&field, bool mandatory,
    std::unique_ptr<const std::string> &&default_value)
    : Body::LeafResource(std::move(name), std::move(module), std::move(parent),
                         std::move(field), false, mandatory,
                         std::move(default_value)),
      Endpoint::LeafResource(std::move(rest_endpoint)),
      read_handler_{std::move(read_handler)} {}

NonConfigLeafResource::NonConfigLeafResource(
    std::function<Response(const char *, Key *, size_t)> read_handler,
    std::string rest_endpoint)
    : Body::LeafResource("", "", nullptr, nullptr, false, false, nullptr),
      Endpoint::LeafResource(std::move(rest_endpoint)),
      read_handler_{std::move(read_handler)} {}

const Response NonConfigLeafResource::Value(const std::string &cube_name,
                                            PerListKeyValues &keys) const {
  auto key_params = KeyListArray::Generate(keys);
  return read_handler_(cube_name.data(), key_params.data(), key_params.size());
}

Response NonConfigLeafResource::Value(const std::string &cube_name,
                                      const nlohmann::json &value,
                                      PerListKeyValues &keys,
                                      Endpoint::Operation operation) {
  throw std::runtime_error("Not implemented");
}

ConfigLeafResource::ConfigLeafResource(
    std::function<Response(const char *, Key *, size_t, const char *)>
        create_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        replace_handler,
    std::function<Response(const char *, Key *, size_t)> delete_handler,
    std::function<Response(const char *, Key *, size_t)> read_handler,
    std::string name, std::string module, std::string rest_endpoint,
    std::shared_ptr<Endpoint::ParentResource> parent,
    std::unique_ptr<Body::JsonBodyField> &&field, bool mandatory,
    std::unique_ptr<const std::string> &&default_value)
    : Body::LeafResource(std::move(name), std::move(module), std::move(parent),
                         std::move(field), false, mandatory,
                         std::move(default_value)),
      NonConfigLeafResource(std::move(read_handler), std::move(rest_endpoint)),
      create_handler_{std::move(create_handler)},
      replace_handler_{std::move(replace_handler)},
      delete_handler_{std::move(delete_handler)} {}

Response ConfigLeafResource::Value(const std::string &cube_name,
                                   const nlohmann::json &value,
                                   PerListKeyValues &keys,
                                   Endpoint::Operation operation) {
  auto key_params = KeyListArray::Generate(keys);
  switch (operation) {
  case Endpoint::Operation::kCreate:
    return create_handler_(cube_name.data(), key_params.data(),
                           key_params.size(), value.dump().data());
  case Endpoint::Operation::kReplace:
    return replace_handler_(cube_name.data(), key_params.data(),
                            key_params.size(), value.dump().data());
  case Endpoint::Operation::kUpdate:
    return update_handler_(cube_name.data(), key_params.data(),
                           key_params.size(), value.dump().data());
  default:
    throw std::runtime_error("Unreachable: fully covered enum");
  }
}
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
