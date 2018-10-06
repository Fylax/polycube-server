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
#ifndef PARSER_ENDPOINT_H
#define PARSER_ENDPOINT_H

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include <memory>
#include "../Resources/CubeManager.h"

class RestServer {
 public:
  explicit RestServer(Pistache::Address &&address, std::size_t thr);
  static const std::shared_ptr<Pistache::Rest::Router> &Router();
  void shutdown();

 private:
  static std::shared_ptr<Pistache::Rest::Router> router_;
  void start();
  void init(std::size_t thr);
  std::unique_ptr<Pistache::Http::Endpoint> httpEndpoint_;
};

#endif  // PARSER_ENDPOINT_H
