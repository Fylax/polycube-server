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
#include "../../../../include/Resources/Data/Lib/ListResource.h"

#include <string>

#include "../../../../include/Resources/Data/Lib/KeyListArray.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {
ListResource::ListResource(
    std::function<Response(const char *, Key *, size_t, const char *)>
        create_entry_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        replace_entry_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        update_entry_handler,
    std::function<Response(const char *, Key *, size_t)> read_entry_handler,
    std::function<Response(const char *, Key *, size_t)> delete_entry_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        create_whole_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        replace_whole_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        update_whole_handler,
    std::function<Response(const char *, Key *, size_t)> read_whole_handler,
    std::function<Response(const char *, Key *, size_t)> delete_whole_handler,
    std::function<Response(const char *, Key *, size_t)> help_handler,
    std::string name, std::string module, std::string rest_endpoint,
    std::string rest_endpoint_multiple,
    std::shared_ptr<Endpoint::ParentResource> parent,
    std::vector<Body::ListKey> &&keys)
    : Body::ParentResource(std::move(name), std::move(module),
                           std::move(parent), false),
      Endpoint::ListResource(std::move(rest_endpoint),
                             std::move(rest_endpoint_multiple),
                             std::move(keys)),
      create_entry_handler_{std::move(create_entry_handler)},
      replace_entry_handler_{std::move(replace_entry_handler)},
      update_entry_handler_{std::move(update_entry_handler)},
      read_entry_handler_{std::move(read_entry_handler)},
      delete_entry_handler_{std::move(delete_entry_handler)},
      create_whole_handler_{std::move(create_whole_handler)},
      replace_whole_handler_{std::move(replace_whole_handler)},
      update_whole_handler_{std::move(update_whole_handler)},
      read_whole_handler_{std::move(read_whole_handler)},
      delete_whole_handler_{std::move(delete_whole_handler)},
      help_handler_{std::move(help_handler)} {}

const Response ListResource::ReadValue(const std::string &cube_name,
                                       PerListKeyValues &keys) const {
  auto key_params = KeyListArray::Generate(keys);
  return read_entry_handler_(cube_name.data(), key_params.data(),
                             key_params.size());
}

Response ListResource::WriteValue(const std::string &cube_name,
                                  const nlohmann::json &value,
                                  PerListKeyValues &keys,
                                  Endpoint::Operation operation) {
  auto key_params = KeyListArray::Generate(keys);
  switch (operation) {
  case Endpoint::Operation::kCreate:
    return create_entry_handler_(cube_name.data(), key_params.data(),
                                 key_params.size(), value.dump().data());
  case Endpoint::Operation::kReplace:
    return replace_entry_handler_(cube_name.data(), key_params.data(),
                                  key_params.size(), value.dump().data());
  case Endpoint::Operation::kUpdate:
    return update_entry_handler_(cube_name.data(), key_params.data(),
                                 key_params.size(), value.dump().data());
  default:
    throw std::runtime_error("Unreachable: fully covered enum");
  }
}
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
