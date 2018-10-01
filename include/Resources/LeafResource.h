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
#ifndef PARSER_LEAFRESOURCE_H
#define PARSER_LEAFRESOURCE_H

#include "Resource.h"
#include <pistache/router.h>
#include <memory>
#include <string>
#include "../Server/Error.h"
#include "JsonBodyField.h"

using Pistache::Rest::Request;
using Pistache::Http::ResponseWriter;

class LeafResource: public Resource {
public:
  LeafResource(const std::string& name, const std::string& module,
               const std::string& rest_endpoint,
               const std::shared_ptr<ParentResource>& parent,
               std::unique_ptr<JsonBodyField>&& field,
               bool configurable, bool mandatory,
               std::unique_ptr<const std::string>&& default_value);

  ~LeafResource();

  std::vector<Response> Validate(const nlohmann::json& body) const final;

  std::vector<Response>
  Validate(const Pistache::Rest::Request& value) const final;

  bool IsMandatory() const final;

  void SetDefaultIfMissing(nlohmann::json& body) const final;

  bool ValidateXPath(const std::string& xpath) const final;

private:
  const std::unique_ptr<JsonBodyField> field_;
  const bool configurable_;
  const bool mandatory_;
  const std::unique_ptr<const std::string> default_;

  void get(const Request& request, ResponseWriter response);

  void post(const Request& request, ResponseWriter response);

  void put(const Request& request, ResponseWriter response);
};


#endif //PARSER_LEAFRESOURCE_H
