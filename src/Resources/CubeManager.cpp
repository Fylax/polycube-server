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
#include "../../include/Resources/CubeManager.h"
#include "../../externals/include/nlohmann/json.hpp"

#include <pistache/router.h>

#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../include/Server/Base64.h"
#include "../../include/Parser/Parser.h"
#include "../../include/Server/ResponseGenerator.h"
#include "../../include/Server/RestServer.h"

std::unordered_set<std::string> CubeManager::existing_cubes_impl_ = {};

bool CubeManager::CreateCube(std::string name) {
  return CubeManager::existing_cubes_impl_.insert(name).second;
}

void CubeManager::RemoveCube(std::string name) {
  CubeManager::existing_cubes_impl_.erase(name);
}

CubeManager::CubeManager(): existing_cubes_() {
  using Pistache::Rest::Routes::bind;
  RestServer::Router()->post("/", bind(&CubeManager::post, this));
}

void CubeManager::post(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response) {
  nlohmann::json body = nlohmann::json::parse(request.body());

  auto yang = Base64::decode(body["model"].get<std::string>());

  if (existing_cubes_.count(Parser::GetName(yang)) != 0) {
    ResponseGenerator::Generate(std::vector<Response>{
        {ErrorTag::kDataExists, ""}
    }, std::move(response));
    return;
  }
  auto cube = Parser::Parse(std::move(yang));
  existing_cubes_[cube->Name()] = cube;
  ResponseGenerator::Generate(std::vector<Response>{{ErrorTag::kCreated, ""}},
                              std::move(response));
}

