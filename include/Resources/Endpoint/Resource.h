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

#include <string>
#include <vector>
#include "../Body/Resource.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
class Resource {
 public:
  explicit Resource(std::string &&rest_endpoint);

  virtual ~Resource() = default;

  const std::string &Endpoint() const;

  virtual std::vector<Response> RequestValidate(
      const Pistache::Rest::Request &request,
      const std::string &caller_name) const = 0;

 protected:
  const std::string rest_endpoint_;

  virtual void CreateReplaceUpdate(const Pistache::Rest::Request& request,
                                   Pistache::Http::ResponseWriter response,
                                   bool check_mandatory) = 0;
};
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
