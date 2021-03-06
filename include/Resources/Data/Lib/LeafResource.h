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

#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "../../Endpoint/LeafResource.h"
#include "EntryPoint.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
class ParentResource;
}

namespace polycube::polycubed::Rest::Resources::Data::Lib {
using ListKeyValues = std::vector<std::pair<Body::ListKey, std::string>>;
using PerListKeyValues = std::stack<ListKeyValues>;

class LeafResource : public Endpoint::LeafResource {
 public:
  LeafResource(
      std::function<Response(const char *, Key *, size_t)> read_handler,
      std::string name, std::string module, std::string rest_endpoint,
      const Body::ParentResource * parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool mandatory,
      std::unique_ptr<const std::string> &&default_value);

  LeafResource(
      std::function<Response(const char *, Key *, size_t, const char *)>
          replace_handler,
      std::function<Response(const char *, Key *, size_t)> read_handler,
      std::string name, std::string module, std::string rest_endpoint,
      const Body::ParentResource * parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool mandatory,
      std::unique_ptr<const std::string> &&default_value);

  const Response ReadValue(const std::string &cube_name,
                           PerListKeyValues &keys) const final;

  Response WriteValue(const std::string &cube_name, const nlohmann::json &value,
                      PerListKeyValues &keys,
                      Endpoint::Operation operation) final;

 private:
  const std::function<Response(const char *, Key *, size_t)> read_handler_;
  const std::function<Response(const char *, Key *, size_t, const char *)>
      replace_handler_;
};
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
