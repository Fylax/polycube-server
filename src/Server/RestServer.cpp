//
// Created by nico on 10/09/18.
//

#include "../../include/Server/RestServer.h"
#include <memory>

RestServer::RestServer(Pistache::Address address): httpEndpoint_(
      std::make_shared<Pistache::Http::Endpoint>(address)) {}

void RestServer::init(std::size_t thr) {
    auto opts = Pistache::Http::Endpoint::options()
        .threads(thr)
        .flags(Pistache::Tcp::Options::InstallSignalHandler);
    httpEndpoint_->init(opts);
  }

  void RestServer::start() {
    httpEndpoint_->setHandler(router_.handler());
    httpEndpoint_->serve();
  }

  void RestServer::shutdown() {
    httpEndpoint_->shutdown();
  }
}