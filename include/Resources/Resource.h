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

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "../Error.h"

class ParentResource;

class Resource {
public:
  Resource(const std::string& name,
           const std::shared_ptr<Pistache::Rest::Router>& router,
           const std::string& restEndpoint,
           const std::shared_ptr<ParentResource>& parent = nullptr);
  virtual bool IsMandatory() const = 0;
protected:
  const std::string name_;
  std::shared_ptr<Pistache::Rest::Router> router_;
  const std::string restEndpoint_;
  std::shared_ptr<ParentResource> parent_;

  virtual Response Validate(const Pistache::Rest::Request& request) const = 0;
};

#endif  // PARSER_RESOURCE_H
