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

#include "../Body/ParentResource.h"
#include "Resource.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
using Pistache::Http::ResponseWriter;
using Pistache::Rest::Request;

class ParentResource : public Resource, public virtual Body::ParentResource {
 public:
  ParentResource(std::string name, std::string module,
                 std::string rest_endpoint,
                 std::shared_ptr<ParentResource> parent,
                 bool configuration,
                 bool container_presence = false);

  ~ParentResource() override;

  std::vector<Response> RequestValidate(
      const Pistache::Rest::Request &request,
      const std::string &caller_name) const override;

  void CreateReplaceUpdate(const Pistache::Rest::Request &request,
                           Pistache::Http::ResponseWriter response,
                           bool update, bool check_mandatory) final;

  void Keys(const Pistache::Rest::Request &request,
            PerListKeyValues &parsed) const override;

 protected:
  /**
   * Used by ChoiceResource and CaseResource: no endpoint and no
   * explicit virtual base construction
   */
  ParentResource();

  /**
   * Used by derived classes: no explicit virtual base construction
   */
  ParentResource(std::string rest_endpoint, bool configuration);

 private:
  const bool has_endpoints_;
  void get(const Request &request, ResponseWriter response);

  virtual void post(const Request &request, ResponseWriter response);

  virtual void put(const Request &request, ResponseWriter response);

  virtual void patch(const Request &request, ResponseWriter response);
};
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
