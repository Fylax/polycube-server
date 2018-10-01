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
#ifndef PARSER_RESOURCE_H
#define PARSER_RESOURCE_H

#include <pistache/router.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "../../externals/include/nlohmann/json.hpp"
#pragma GCC diagnostic pop

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <shared_mutex>

#include "../Server/Error.h"

class ParentResource;

class Resource {
public:
  Resource(const std::string& name, const std::string& module,
           const std::string& rest_endpoint,
           const std::shared_ptr<ParentResource>& parent);

  virtual ~Resource() = default;

  virtual bool IsMandatory() const = 0;

  const std::string& Name() const;

  const std::string& ModuleName() const;

  const std::string& Endpoint() const;

  virtual bool ValidateXPath(const std::string& xpath) const = 0;

  virtual std::vector<Response> Validate(const nlohmann::json& body) const = 0;

  virtual std::vector<Response>
  Validate(const Pistache::Rest::Request& request) const = 0;

  virtual void SetDefaultIfMissing(nlohmann::json& body) const = 0;

protected:
  const std::string name_;
  const std::string rest_endpoint_;
  std::shared_ptr<ParentResource> parent_;
  const std::string module_;
};

#endif  // PARSER_RESOURCE_H
