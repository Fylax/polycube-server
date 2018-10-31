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

#include <pistache/router.h>

#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "../../Validators/InSetValidator.h"
#include "../Body/Service.h"
#include "ParentResource.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
using Pistache::Http::ResponseWriter;
using Pistache::Rest::Request;

class Service : public ParentResource, public Body::Service {
 public:
  Service(const std::string &name, std::string base_address);

  ~Service() override;

  using Body::Service::ValidateXPath;

  static const std::string Cube(const Pistache::Rest::Request &request);

  bool HasCubes() const;

 private:
  const std::string body_rest_endpoint_;
  Validators::InSetValidator path_param_;

  void CreateReplaceUpdate(const std::string &name, nlohmann::json body,
                           ResponseWriter response, bool replace, bool check_mandatory);

  std::vector<Response> RequestValidate(
      const Pistache::Rest::Request &request,
      const std::string &caller_name) const final;

  void post(const Request &request, ResponseWriter response) final;

  void put(const Request &request, ResponseWriter response) final;

  void patch(const Request &request, ResponseWriter response) final;

  void del(const Request &request, ResponseWriter response) final;

  void get_body(const Request &request, ResponseWriter response);

  void post_body(const Request &request, ResponseWriter response);

  void patch_body(const Request &request, ResponseWriter response);
};
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
