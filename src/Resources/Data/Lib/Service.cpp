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

#include <Lib/Service.h>

#include "../../../../include/Resources/Data/Lib/Service.h"
namespace polycube::polycubed::Rest::Resources::Data::Lib {

Service::Service(
    std::function<Response(const char *, const char *)> create_handler,
    std::function<Response(const char *, const char *)> update_handler,
    std::function<Response(const char *)> read_handler,
    std::function<Response(const char *, const char *)> replace_handler,
    std::function<Response(const char *)> delete_handler,
    std::function<Response(void)> read_list_handler,
    std::function<Response(void)> help_handler,
    std::function<Response(const char *)> update_list_handler, std::string name,
    std::string base_address)
    : Body::ParentResource(name, name, nullptr, true),
      Endpoint::Service(name, std::move(base_address)),
      create_handler_{std::move(create_handler)},
      update_handler_{std::move(update_handler)},
      read_handler_{std::move(read_handler)},
      replace_handler_{std::move(replace_handler)},
      delete_handler_{std::move(delete_handler)},
      read_list_handler_{std::move(read_list_handler)},
      help_handler_{std::move(help_handler)},
      update_list_handler_{std::move(update_list_handler)} {}

const Response Service::ReadValue(
    const std::string &cube_name,
    [[maybe_unused]] PerListKeyValues &keys) const {
  return read_handler_(cube_name.data());
}

Response Service::WriteValue(const std::string &cube_name,
                             const nlohmann::json &value,
                             [[maybe_unused]] PerListKeyValues &keys,
                             Endpoint::Operation operation) {
  switch (operation) {
  case Endpoint::Operation::kCreate:
    return create_handler_(cube_name.data(), value.dump().data());
  case Endpoint::Operation::kReplace:
    return replace_handler_(cube_name.data(), value.dump().data());
  case Endpoint::Operation::kUpdate:
    return update_handler_(cube_name.data(), value.dump().data());
  default:
    throw std::runtime_error("Unreachable: fully covered enum.");
  }
}

}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
