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
#include "../../include/Server/RestServer.h"
#include "../../include/Resources/ServiceManager.h"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <memory>

namespace polycube::polycubed::Rest::Server {
std::shared_ptr<Pistache::Rest::Router> RestServer::router_ = nullptr;

RestServer::RestServer(Pistache::Address &&address, std::size_t thr)
    : httpEndpoint_(std::make_unique<Pistache::Http::Endpoint>(address)) {
  RestServer::router_ = std::make_shared<Pistache::Rest::Router>();
  init(thr);
  start();
}

const std::shared_ptr<Pistache::Rest::Router> &RestServer::Router() {
  return router_;
}

void RestServer::shutdown() {
  httpEndpoint_->shutdown();
}

void RestServer::init(std::size_t thr) {
  auto opts = Pistache::Http::Endpoint::options().threads(thr).flags(
      Pistache::Tcp::Options::InstallSignalHandler);
  httpEndpoint_->init(opts);
  Resources::ServiceManager::GetInstance();
}

void RestServer::start() {
  httpEndpoint_->setHandler(Pistache::Rest::Router::handler(router_));
  httpEndpoint_->serve();
}
}  // namespace polycube::polycubed::Rest::Server
