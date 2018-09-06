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

#include "../../include/Resources/Resource.h"

#include <pistache/router.h>
#include <memory>
#include <string>
#include <vector>

#include "../../include/Error.h"

Resource::Resource(const std::string& name,
                   const std::shared_ptr<Pistache::Rest::Router>& router,
                   const std::string& restEndpoint,
                   const std::shared_ptr<ParentResource>& parent):
    name_(name), router_(router), restEndpoint_(restEndpoint),
    parent_(parent) {}

const std::string& Resource::Endpoint() const {
  return restEndpoint_;
}
