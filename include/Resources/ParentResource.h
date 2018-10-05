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
#ifndef PARSER_PARENTRESOURCE_H
#define PARSER_PARENTRESOURCE_H

#include <pistache/router.h>

#include <list>
#include <string>
#include <memory>
#include <vector>

#include "../Server/Error.h"
#include "Resource.h"
#include "PathParamField.h"

using Pistache::Rest::Request;
using Pistache::Http::ResponseWriter;

class ParentResource: public Resource {
public:
  ParentResource(std::string name, std::string module,
                 std::string rest_endpoint,
                 std::shared_ptr<ParentResource> parent,
                 std::vector<PathParamField>&& fields,
                 bool container_presence = false);

  ~ParentResource() override;

  std::vector<Response>
  Validate(const nlohmann::json& body) const final;

  std::vector<Response>
  Validate(const Request& value,
           const std::string& caller_name) const override;

  virtual void AddChild(std::shared_ptr<Resource> child);

  bool IsMandatory() const final;

  void SetDefaultIfMissing(nlohmann::json& body) const override;

  bool ValidateXPath(const std::string& xpath) const override;

protected:
  std::vector<PathParamField> fields_;
  std::vector<std::shared_ptr<Resource>> children_;

  bool ValidateXPathChildren(const std::string& xpath,
                             std::size_t delimiter) const;

  void
  CreateOrReplace(const Request& request, ResponseWriter response) override;

private:
  /**
   * MUST be set to true only if the parent is a container
   * and it has no presence flag (or explicitly set to false).
   */
  const bool container_presence_;

  void get(const Request& request, ResponseWriter response);

  virtual void post(const Request& request, ResponseWriter response);

  virtual void put(const Request& request, ResponseWriter response);

  virtual void patch(const Request& request, ResponseWriter response);
};


#endif  //PARSER_PARENTRESOURCE_H
