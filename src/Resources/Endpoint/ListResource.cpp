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
#include "../../../include/Resources/Endpoint/ListResource.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {
ListResource::ListResource(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent, std::string rest_endpoint,
    std::vector<std::pair<std::string,
                          std::vector<std::shared_ptr<Validators::Validator>>>>
        &&keys)
    : Body::ParentResource(std::move(name), std::move(module),
                           std::move(parent), false),
      ParentResource(std::move(rest_endpoint)),
      Body::ListResource(std::move(keys)),
      key_params_{} {
  for (const auto &key : keys_) {
    key_params_.emplace_back(key.first, key.second);
  }
}

std::vector<Response> ListResource::RequestValidate(
    const Pistache::Rest::Request &request,
    const std::string &caller_name) const {
  auto errors = ParentResource::RequestValidate(request, caller_name);
  for (const auto &key_param : key_params_) {
    auto error = key_param.Validate(request);
    if (error != ErrorTag::kOk) {
      errors.push_back({error, key_param.Name().data()});
    }
  }
  return errors;
}
}  // namespace polycube::polycubed::Rest::Resources::Endpoint
