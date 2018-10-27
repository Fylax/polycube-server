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

#include "../../../../include/Resources/Body/ListKey.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {

ParentResource::ParentResource(
    std::function<Response(const char *, Key *, size_t, const char *)>
        create_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        replace_handler,
    std::function<Response(const char *, Key *, size_t, const char *)>
        update_handler,
    std::function<Element(const char *, Key *, size_t)> read_handler,
    std::function<Response(const char *, Key *, size_t)> delete_handler,
    std::string name, std::string module, std::string rest_endpoint,
    std::shared_ptr<Endpoint::ParentResource> parent, bool container_presence)
    : Body::ParentResource(std::move(name), std::move(module),
                           std::move(parent), container_presence),
      Endpoint::ParentResource(std::move(rest_endpoint)),
      create_handler_{std::move(create_handler)},
      replace_handler_{std::move(replace_handler)},
      update_handler_{std::move(update_handler)},
      read_handler_{std::move(read_handler)},
      delete_handler_{std::move(delete_handler)} {}

const nlohmann::json ParentResource::Value(const std::string &cube_name,
                                           PerListKeyValues &keys) const {
  std::vector<Key> key_params;
  while (!keys.empty()) {
    const auto &list_keys = keys.top();
    keys.pop();
    for (const auto &key : list_keys) {
      const auto &lk = key.first;
      key_params.push_back()
    }
  }
  return Resource::Value(cube_name, keys);
}
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
