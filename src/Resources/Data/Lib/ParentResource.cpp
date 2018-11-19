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
#include "../../../../include/Resources/Data/Lib/ParentResource.h"

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <Lib/ParentResource.h>


#include "../../../../include/Resources/Body/ListKey.h"
#include "../../../../include/Resources/Data/Lib/KeyListArray.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {

ParentResource::ParentResource(
    std::function<Response(const char *, Key *, size_t, const char *)>
        create_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        replace_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        update_handler,
    std::function<Response(const char *, Key *, size_t)> read_handler,
    std::function<Response(const char *, Key *, size_t)> delete_handler,
    std::string name, std::string module, std::string rest_endpoint,
    const Body::ParentResource * const parent, bool container_presence)
    : Body::ParentResource(std::move(name), std::move(module),
                           parent, true, container_presence),
      Endpoint::ParentResource(std::move(rest_endpoint), true, false),
      create_handler_{std::move(create_handler)},
      replace_handler_{std::move(replace_handler)},
      update_handler_{std::move(update_handler)},
      read_handler_{std::move(read_handler)},
      delete_handler_{std::move(delete_handler)} {}

ParentResource::ParentResource(
    std::function<Response(const char *, Key *, size_t, const char *)>
        create_handler,
    std::string name, std::string module, std::string rest_endpoint,
    const Body::ParentResource * const parent)
    : Body::ParentResource(std::move(name), std::move(module),
                           parent, false, false),
      Endpoint::ParentResource(std::move(rest_endpoint), false, true),
      create_handler_{std::move(create_handler)},
      replace_handler_{},
      update_handler_{},
      read_handler_{},
      delete_handler_{} {}

ParentResource::ParentResource(
    std::function<Response(const char *, Key *, size_t)> read_handler,
    std::string name, std::string module, std::string rest_endpoint,
    const Body::ParentResource * const parent, bool container_presence)
    : Body::ParentResource(std::move(name), std::move(module),
                           parent, false, container_presence),
      Endpoint::ParentResource(std::move(rest_endpoint), false, false),
      create_handler_{},
      replace_handler_{},
      update_handler_{},
      read_handler_{std::move(read_handler)},
      delete_handler_{} {}

const Response ParentResource::ReadValue(const std::string &cube_name,
                                         PerListKeyValues &keys) const {
  auto key_params = KeyListArray::Generate(keys);
  return read_handler_(cube_name.data(), key_params.data(), key_params.size());
}

Response ParentResource::WriteValue(const std::string &cube_name,
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

Response ParentResource::DeleteValue(const std::string &cube_name,
                                     PerListKeyValues &keys) {
  auto key_params = KeyListArray::Generate(keys);
  return delete_handler_(cube_name.data(), key_params.data(), key_params.size());
}
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
