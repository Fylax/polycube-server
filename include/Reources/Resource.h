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

#include "Field.h"

class Resource {
 public:
  Resource(std::shared_ptr<Pistache::Rest::Router> router,
           const std::string& restEndpoint,
           const std::vector<Field>& fields);

  virtual ~Resource();

private:
  std::shared_ptr<Pistache::Rest::Router> router_;
  const std::string restEndpoint_;
  const std::vector<Field> fields_;

  void get(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter);

  void post(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter);

  void put(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter);

  void patch(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter);

  void del(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter);
};

#endif //PARSER_RESOURCE_H
