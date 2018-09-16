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
#ifndef PARSER_CUBES_H
#define PARSER_CUBES_H

#include <pistache/router.h>

#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Cube.h"

class CubeManager {
public:
  static bool CreateCube(std::string name);
  static void RemoveCube(std::string name);

  explicit CubeManager(const std::shared_ptr<Pistache::Rest::Router>& router);


private:
  static std::unordered_set<std::string> existing_cubes_impl_;
  std::unordered_map<std::string, std::shared_ptr<Cube>> existing_cubes_;
  std::shared_ptr<Pistache::Rest::Router> router_;
  void post(const Pistache::Rest::Request& request,
      Pistache::Http::ResponseWriter response);
};


#endif //PARSER_CUBES_H
