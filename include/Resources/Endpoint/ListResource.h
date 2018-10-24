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
#include <string>
#include <vector>
#include <utility>

#include "../Body/ListResource.h"
#include "ParentResource.h"

namespace polycube::polycubed::Rest::Resources::Body {
class ListKey;
}

namespace polycube::polycubed::Rest::Resources::Endpoint {
class PathParamField;

class ListResource : public ParentResource, public Body::ListResource {
 public:
  ListResource(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent, std::string rest_endpoint,
      std::string rest_endpoint_multiple,
      std::vector<Body::ListKey> &&keys);

  ~ListResource() override;

  using Body::ListResource::ValidateXPath;

  std::vector<Response> RequestValidate(
      const Pistache::Rest::Request &request,
      const std::string &caller_name) const final;

 private:
  std::vector<PathParamField> key_params_;
  std::string multiple_endpoint_;

  void get_multiple(const Request &request, ResponseWriter response);

  void post_multiple(const Request &request, ResponseWriter response);

  void put_multiple(const Request &request, ResponseWriter response);

  void patch_multiple(const Request &request, ResponseWriter response);
};
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
