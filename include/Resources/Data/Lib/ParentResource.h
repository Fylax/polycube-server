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
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "../../Endpoint/ParentResource.h"
#include "EntryPoint.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {
using ListKeyValues = std::vector<std::pair<Body::ListKey, std::string>>;
using PerListKeyValues = std::stack<ListKeyValues>;

class ParentResource : public Endpoint::ParentResource {
 public:
  ParentResource(
      std::function<Response(const char *, Key *, size_t, const char *)>
          create_handler,
      std::function<Response(const char *, Key *, size_t, const char *)>
          replace_handler,
      std::function<Response(const char *, Key *, size_t, const char *)>
          update_handler,
      std::function<Response(const char *, Key *, size_t)> read_handler,
      std::function<Response(const char *, Key *, size_t)> delete_handler,
      std::string name, std::string module, std::string rest_endpoint,
      std::shared_ptr<Endpoint::ParentResource> parent,
      bool container_presence = false);

  const Response Value(const std::string &cube_name,
                       PerListKeyValues &keys) const final;

  Response Value(const std::string &cube_name, const nlohmann::json &value,
                 PerListKeyValues &keys,
                 Endpoint::Operation operation) final;

 private:
  const std::function<Response(const char *, Key *, size_t, const char *)>
      create_handler_;
  const std::function<Response(const char *, Key *, size_t, const char *)>
      replace_handler_;
  const std::function<Response(const char *, Key *, size_t, const char *)>
      update_handler_;
  const std::function<Response(const char *, Key *, size_t)> read_handler_;
  const std::function<Response(const char *, Key *, size_t)> delete_handler_;
};
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
