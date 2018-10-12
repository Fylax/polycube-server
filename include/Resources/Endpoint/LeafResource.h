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

#include "../Body/JsonBodyField.h"
#include "../Body/LeafResource.h"
#include "ParentResource.h"
#include "Resource.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
using Pistache::Http::ResponseWriter;
using Pistache::Rest::Request;
class LeafResource : public Resource, public virtual Body::LeafResource {
 public:
  LeafResource(std::string name, std::string module, std::string rest_endpoint,
               std::shared_ptr<ParentResource> parent,
               std::unique_ptr<Body::JsonBodyField> &&field, bool configurable,
               bool mandatory,
               std::unique_ptr<const std::string> &&default_value);

  ~LeafResource() override;

  std::vector<Response> RequestValidate(
      const Request &request, const std::string &caller_name) const override;

protected:
  explicit LeafResource(std::string rest_endpoint);
 private:
  void get(const Request &request, ResponseWriter response);

  void CreateReplaceUpdate(const Pistache::Rest::Request &request,
                           Pistache::Http::ResponseWriter response,
                           bool check_mandatory) final;

  void post(const Request &request, ResponseWriter response);

  void put(const Request &request, ResponseWriter response);
};
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
