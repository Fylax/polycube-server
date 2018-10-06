#include <pistache/http.h>

#include <string>
#include <utility>
#include <vector>
#include "include/Parser/Parser.h"
#include "include/Server/RestServer.h"

int main() {
  Pistache::Port port{9080};
  Pistache::Address address{Pistache::Ipv4::any(), port};
  RestServer server{std::move(address), 1};

  server.shutdown();
  return 0;
}
