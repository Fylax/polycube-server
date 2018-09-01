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

#include "../../include/Reources/Resource.h"

#include <pistache/router.h>
#include <memory>
#include <string>
#include <vector>

Resource::Resource(std::shared_ptr<Pistache::Rest::Router> router,
                   const std::string& restEndpoint,
                   const std::vector<Field>& fields): router_(router),
                   restEndpoint_(restEndpoint),
                   fields_(fields) {
  using Pistache::Rest::Routes::bind;
  router_->get(restEndpoint_, bind(&Resource::get, this));
  router_->post(restEndpoint_, bind(&Resource::post, this));
  router_->put(restEndpoint_, bind(&Resource::put, this));
  router_->patch(restEndpoint_, bind(&Resource::patch, this));
  router_->del(restEndpoint_, bind(&Resource::del, this));
}

Resource::~Resource() {
  using Pistache::Http::Method;
  router_->removeRoute(Method::Get, restEndpoint_);
  router_->removeRoute(Method::Post, restEndpoint_);
  router_->removeRoute(Method::Put, restEndpoint_);
  router_->removeRoute(Method::Patch, restEndpoint_);
  router_->removeRoute(Method::Delete, restEndpoint_);
}
