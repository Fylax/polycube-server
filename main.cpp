#include <libyang/libyang.h>

#include "include/Parser/Parser.h"


int main() {
  auto context = ly_ctx_new("/home/nico/dev/iovnet/services/resources/", 0);
  auto added = ly_ctx_set_searchdir(context,
                                   "/home/nico/dev/iovnet/services/iov-helloworld/resources");
  if (!context || added == EXIT_FAILURE) {
    throw std::runtime_error("cannot create new context");
  }

  auto module = ly_ctx_get_module(context, "helloworld",
                                  nullptr, true);
  if (!module) {
    module = ly_ctx_load_module(context, "helloworld",
                                nullptr);
    if (!module) {
      throw std::runtime_error("cannot open yang file");
    }
  }

  for (auto i = 0; i < module->imp_size; ++i) {
    parseModule(module->imp[i].module);
  }
  parseModule(module);

  return 0;
}

