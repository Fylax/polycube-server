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
#ifndef PARSER_CUBE_H
#define PARSER_CUBE_H

#include <pistache/router.h>

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ParentResource.h"

using Pistache::Http::ResponseWriter;
using Pistache::Rest::Request;

class Cube : public ParentResource {
 public:
  Cube(const std::string &name, std::string base_address);

  ~Cube() final;

  bool ValidateXPath(const std::string &xpath) const final;

 private:
  const std::string body_rest_endpoint_;
  std::unordered_map<std::string, std::shared_mutex> mutex_;

  void CreateOrReplace(const std::string &name, const nlohmann::json &body,
                       ResponseWriter response, bool replace);

  void post(const Request &request, ResponseWriter response) final;

  void put(const Request &request, ResponseWriter response) final;

  void patch(const Request &request, ResponseWriter response) final;

  void get_body(const Request &request, ResponseWriter response);

  void post_body(const Request &request, ResponseWriter response);

  void patch_body(const Request &request, ResponseWriter response);
};

#endif  // PARSER_CUBE_H
