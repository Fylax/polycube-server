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
    : mutex_{}, services_{}, cubes_{} {
  using Pistache::Rest::Routes::bind;
  Server::RestServer::Router()->post("/", bind(&ServiceManager::post, this));
  Server::RestServer::Router()->del("/", bind(&ServiceManager::del, this));
}

bool ServiceManager::ExistsCube(const std::string &name) const {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  return ServiceManager::cubes_.count(name) != 0;
}

std::shared_ptr<Body::Resource> ServiceManager::Cube(
    const std::string &service, const std::string &name) const {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  if (ServiceManager::services_.count(service) == 0)
    return nullptr;
  return ServiceManager::services_.at(service)->Child(name);
}

bool ServiceManager::CreateCube(const std::string &name) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  return ServiceManager::cubes_.insert(name).second;
}

void ServiceManager::RemoveCube(const std::string &name) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  cubes_.erase(name);
}

bool ServiceManager::ValidateXpath(const std::string &xpath,
                                   const std::string &context) const {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  if (services_.count(context) == 0)
    return false;
  return services_.at(context)->ValidateXPath(xpath);
}

void ServiceManager::post(const Pistache::Rest::Request &request,
                          Pistache::Http::ResponseWriter response) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  auto start = std::chrono::high_resolution_clock::now();
  try {
    nlohmann::json body = nlohmann::json::parse(request.body());
    auto factory = Data::AbstractFactory::Concrete(body);
    auto service_name = Parser::Yang::ServiceName(factory->Yang());
    try {
      if (services_.count(service_name) != 0) {
        Server::ResponseGenerator::Generate(
            std::vector<Response>{{ErrorTag::kDataExists, ""}},
            std::move(response));
        return;
      }

      services_[service_name] = Parser::Yang(std::move(factory)).Parse();
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::nanoseconds diff = end - start;
      Server::ResponseGenerator::Generate(
          std::vector<Response>{{ErrorTag::kCreated, std::to_string(diff.count()).data()}}, std::move(response));
    } catch (const std::invalid_argument &e) {
      Data::AbstractFactory::RemoveService(service_name);
      response.send(Pistache::Http::Code::Bad_Request, e.what());
    }
  } catch (const std::exception &e) {
    response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
  }
}

void ServiceManager::del(const Pistache::Rest::Request &request,
                          Pistache::Http::ResponseWriter response) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  auto body = nlohmann::json::parse(request.body());
  auto name = body["name"].get<std::string>();
  if (services_.count(name) == 0) {
    Server::ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kBadAttribute, ":name"}},
        std::move(response));
    return;
  }
  const auto &service = services_.at(name);
  if (service->HasCubes()) {
    Server::ResponseGenerator::Generate(
        std::vector<Response>{{ErrorTag::kBadAttribute, ":name"}},
        std::move(response));
    return;
  }
  services_.erase(name);
  Data::AbstractFactory::RemoveService(name);
  Server::ResponseGenerator::Generate(
      std::vector<Response>{{ErrorTag::kNoContent, ""}},
      std::move(response));
}
}  // namespace polycube::polycubed::Rest::Resources
