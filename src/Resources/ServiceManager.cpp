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
#include "../../include/Resources/ServiceManager.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

#pragma GCC diagnostic pop

#include "../../include/Parser/Yang.h"
#include "../../include/Resources/Data/AbstractFactory.h"
#include "../../include/Server/Base64.h"
#include "../../include/Server/ResponseGenerator.h"
#include "../../include/Server/RestServer.h"

namespace polycube::polycubed::Rest::Resources {
ServiceManager::ServiceManager()
    : mutex_{}, existing_cubes_{}, existing_cubes_impl_{} {
  using Pistache::Rest::Routes::bind;
  Server::RestServer::Router()->post("/", bind(&ServiceManager::post, this));
}

bool ServiceManager::ExistsCube(const std::string &name) const {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  return ServiceManager::existing_cubes_impl_.count(name) != 0;
}

std::shared_ptr<Body::Resource> ServiceManager::Cube(
    const std::string &service, const std::string &name) const {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  if (ServiceManager::existing_cubes_.count(service) == 0)
    return nullptr;
  return ServiceManager::existing_cubes_.at(service)->Child(name);
}

bool ServiceManager::CreateCube(const std::string &name) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  return ServiceManager::existing_cubes_impl_.insert(name).second;
}

void ServiceManager::RemoveCube(const std::string &name) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  existing_cubes_impl_.erase(name);
}

bool ServiceManager::ValidateXpath(const std::string &xpath,
                                const std::string &context) const {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  if (existing_cubes_.count(context) == 0)
    return false;
  return existing_cubes_.at(context)->ValidateXPath(xpath);
}

void ServiceManager::post(const Pistache::Rest::Request &request,
                       Pistache::Http::ResponseWriter response) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  try {
    nlohmann::json body = nlohmann::json::parse(request.body());
    auto factory = Data::AbstractFactory::Concrete(body);

    if (existing_cubes_.count(Parser::Yang::ServiceName(factory->Yang())) != 0) {
      Server::ResponseGenerator::Generate(
          std::vector<Response>{{ErrorTag::kDataExists, ""}},
          std::move(response));
      return;
    }

    auto cube = Parser::Yang(std::move(factory)).Parse();
    existing_cubes_[cube->Name()] = cube;
    Server::ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kCreated, ""}}, std::move(response));
  } catch (const std::invalid_argument &e) {
    response.send(Pistache::Http::Code::Bad_Request, e.what());
  } catch (...) {
    response.send(Pistache::Http::Code::Internal_Server_Error);
  }
}
}  // namespace polycube::polycubed::Rest::Resources
