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
#pragma once

#include <pistache/router.h>

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "Endpoint/Service.h"

namespace polycube::polycubed::Rest::Resources {
class CubeManager {
 public:
  static CubeManager &GetInstance() {
    static CubeManager instance;
    return instance;
  }

  CubeManager(CubeManager const &) = delete;

  void operator=(CubeManager const &) = delete;

  bool ExistsCube(const std::string &name) const;

  std::shared_ptr<Body::Resource> Cube(const std::string &service,
                                       const std::string &name) const;

  bool CreateCube(const std::string &name);

  void RemoveCube(const std::string &name);

  /**
   * Validates an XPath in a given context.
   * @param xpath XPath to validate.
   * @param context Context (service name).
   * @return Whether the XPath is valid
   */
  bool ValidateXpath(const std::string &xpath,
                     const std::string &context) const;

 private:
  mutable std::shared_mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<Endpoint::Service>>
      existing_cubes_;
  std::unordered_set<std::string> existing_cubes_impl_;

  CubeManager();

  void post(const Pistache::Rest::Request &request,
            Pistache::Http::ResponseWriter response);
};
}  // namespace polycube::polycubed::Rest::Resources
