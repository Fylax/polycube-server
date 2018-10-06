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
#ifndef SERVICE_CONTROLLER_CHOICERESOURCE_H
#define SERVICE_CONTROLLER_CHOICERESOURCE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "LeafResource.h"
#include "ParentResource.h"
#include "PathParamField.h"

class ChoiceResource : public ParentResource {
 public:
  ChoiceResource(std::string name, std::string module,
                 std::string rest_endpoint,
                 std::shared_ptr<ParentResource> parent, bool mandatory,
                 std::unique_ptr<const std::string> &&default_case);

  std::vector<Response> Validate(const Request &request,
                                 const std::string &caller_name) const final;

  void AddChild(std::shared_ptr<Resource> child) final;

  void SetDefaultIfMissing(nlohmann::json &body) const final;

 private:
  const bool mandatory_;
  std::unordered_map<std::string, std::shared_ptr<Resource>> children_;
  const std::unique_ptr<const std::string> default_case_;

  void CreateOrReplace(const Request &request, ResponseWriter response) final;
};

#endif  // SERVICE_CONTROLLER_CHOICERESOURCE_H
