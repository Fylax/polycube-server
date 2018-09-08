#include <libyang/libyang.h>

#include <vector>
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

  // module_res will be Cube/Service
  auto module_res = ParentResource(module->name, nullptr,
      std::string{'/'} + module->name + "/:cube_name/",
      nullptr, std::vector<PathParamField>());
  for (auto i = 0; i < module->imp_size; ++i) {
    parseModule(module->imp[i].module, module_res);
  }
  parseModule(module, module_res);

  return 0;
}

