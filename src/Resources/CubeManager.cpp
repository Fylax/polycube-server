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

#include <mutex>
#include <shared_mutex>

#include "../../include/Server/Base64.h"
#include "../../include/Parser/Parser.h"
#include "../../include/Server/ResponseGenerator.h"
#include "../../include/Server/RestServer.h"


CubeManager::CubeManager(): existing_cubes_{}, existing_cubes_impl_{} {
  using Pistache::Rest::Routes::bind;
  RestServer::Router()->post("/", bind(&CubeManager::post, this));
}

bool CubeManager::CreateCube(const std::string& name) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  return CubeManager::existing_cubes_impl_.insert(name).second;
}

void CubeManager::RemoveCube(const std::string& name) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  existing_cubes_impl_.erase(name);
}

bool CubeManager::ValidateXpath(const std::string& xpath,
                                const std::string& context) const {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  if (existing_cubes_.count(context) == 0) return false;
  return existing_cubes_.at(context)->ValidateXPath(xpath);
}

void CubeManager::post(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response) {
  std::unique_lock<std::shared_mutex> lock(mutex_);

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

