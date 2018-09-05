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

#include <string>
#include <memory>
#include <vector>

#include "../Error.h"
#include "Resource.h"
#include "PathParamField.h"

class ParentResource: public Resource {
 public:
  ParentResource(const std::string& name,
                 const std::shared_ptr<Pistache::Rest::Router>& router,
                 const std::string& restEndpoint,
                 const std::shared_ptr<ParentResource>& parent,
                 bool container_presence = true);
  Response Validate(const Pistache::Rest::Request& value) const override;
  void AddChild(const std::shared_ptr<Resource>& child);
  bool IsMandatory() const override;
 private:
  std::vector<std::shared_ptr<Resource>> children_;
  std::vector<PathParamField> fields_;
  /**
   * MUST be set to false only if the parent is a container
   * and it has no presence flag (or explicitly set to false).
   */
  const bool container_presence_;
};


#endif  //PARSER_PARENTRESOURCE_H
