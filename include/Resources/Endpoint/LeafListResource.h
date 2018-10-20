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
#include "../Body/LeafListResource.h"
#include "LeafResource.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
using Pistache::Http::ResponseWriter;
using Pistache::Rest::Request;

class LeafListResource : public LeafResource, public Body::LeafListResource {
 public:
  LeafListResource(std::string name, std::string module,
                   std::string rest_endpoint,
                   std::shared_ptr<ParentResource> parent,
                   std::unique_ptr<Body::JsonBodyField> &&field,
                   bool configuration, bool mandatory,
                   std::vector<std::string> &&default_value);

  ~LeafListResource() override;

 private:
  void get_entry(const Request &request, ResponseWriter response);
};
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
