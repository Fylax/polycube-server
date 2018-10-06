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

#include <pistache/router.h>
#include <memory>
#include <string>
#include <unordered_map>
#include "../Server/Error.h"
#include "../Validators/InSetValidator.h"
#include "JsonBodyField.h"
#include "Resource.h"

using Pistache::Http::ResponseWriter;
using Pistache::Rest::Request;

class LeafResource : public Resource {
 public:
  LeafResource(std::string name, std::string module, std::string rest_endpoint,
               std::shared_ptr<ParentResource> parent,
               std::unique_ptr<JsonBodyField> &&field, bool configurable,
               bool mandatory,
               std::unique_ptr<const std::string> &&default_value);

  ~LeafResource() override;

  std::vector<Response> Validate(const nlohmann::json &body) const override;

  std::vector<Response> Validate(const Pistache::Rest::Request &value,
                                 const std::string &caller_name) const final;

  bool IsMandatory() const final;

  void SetDefaultIfMissing(nlohmann::json &body) const override;

  bool ValidateXPath(const std::string &xpath) const final;

 private:
  const std::unique_ptr<JsonBodyField> field_;
  const bool configurable_;
  const bool mandatory_;
  const std::unique_ptr<const std::string> default_;

  void get(const Request &request, ResponseWriter response);

  void CreateOrReplace(const Request &request, ResponseWriter response) final;

  void post(const Request &request, ResponseWriter response);

  void put(const Request &request, ResponseWriter response);
};

#endif  // PARSER_LEAFRESOURCE_H
