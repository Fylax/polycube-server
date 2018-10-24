#include <pistache/http.h>

#include <string>
#include <utility>
#include <vector>
#include "include/Parser/Yang.h"
#include "include/Server/RestServer.h"

int main() {
  Pistache::Port port{9080};
  Pistache::Address address{Pistache::Ipv4::any(), port};
  polycube::polycubed::Rest::Server::RestServer server{std::move(address), 1};

  server.shutdown();
  return 0;
}
