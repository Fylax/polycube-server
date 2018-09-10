//
// Created by nico on 10/09/18.
//

#ifndef PARSER_ENDPOINT_H
#define PARSER_ENDPOINT_H

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include <memory>

class RestServer {
public:
  static RestServer& getInstance(std::shared_ptr<Pistache::Address> address){
    static RestServer s(*address);
    return s;
  }

  Pistache::Rest::Router& Router();
  void init(std::size_t thr = 2);
  void start();
  void shutdown();

private:
  explicit RestServer(Pistache::Address address);
  std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint_;
  Pistache::Rest::Router router_;
}


#endif //PARSER_ENDPOINT_H
