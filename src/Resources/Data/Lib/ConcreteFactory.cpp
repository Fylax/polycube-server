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
#include "../../../../include/Resources/Data/Lib/ConcreteFactory.h"

#include <dlfcn.h>

#include <memory>
#include <string>
#include <vector>

#include "../../../../include/Resources/Data/Lib/EntryPoint.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib {
ConcreteFactory::ConcreteFactory(const std::string &file_name)
    : handle_(::dlopen(file_name.data(), RTLD_LAZY),
              [](void *h) { ::dlclose(h); }) {
  if (!handle_) {
    throw std::logic_error("Cannot load service implementation " + file_name);
  }
}

const std::string ConcreteFactory::Yang() const {
  return std::invoke(LoadHandler<const char *(void)>("data_model"));
}

template <typename T>
std::function<T> ConcreteFactory::LoadHandler(
    const std::string &function_name) const {
  void *result = ::dlsym(handle_.get(), function_name.data());
  if (!result) {
    char *const error = ::dlerror();
    if (error) {
      throw std::domain_error("Cannot find handler " + function_name);
    }
  }
  return reinterpret_cast<T *>(result);
}

}  // namespace polycube::polycubed::Rest::Resources::Data::Lib
