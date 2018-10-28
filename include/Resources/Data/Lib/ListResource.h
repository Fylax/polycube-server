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

#include "../../Endpoint/ListResource.h"
#include "EntryPoint.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {
class ListResource : public Endpoint::ListResource {
  ListResource(
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
      std::shared_ptr<ParentResource> parent,
      std::vector<Body::ListKey> &&keys);
};
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
