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
#ifndef SERVICE_CONTROLLER_LEAFLISTRESOURCE_H
#define SERVICE_CONTROLLER_LEAFLISTRESOURCE_H


#include "LeafResource.h"
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

class LeafListResource: public LeafResource {
public:
  LeafListResource(const std::string& name, const std::string& module,
                   const std::string& rest_endpoint,
                   const std::shared_ptr<ParentResource>& parent,
                   std::unique_ptr<JsonBodyField>&& field,
                   bool configurable, bool mandatory,
                   std::vector<std::string>&& default_value);

  ~LeafListResource() override;

  std::vector<Response> Validate(const nlohmann::json& body) const final;

  void SetDefaultIfMissing(nlohmann::json& body) const final;

private:
  const std::vector<std::string> default_;

  void get_entry(const Request& request, ResponseWriter response);
};


#endif //SERVICE_CONTROLLER_LEAFLISTRESOURCE_H
