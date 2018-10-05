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
#ifndef PARSER_LISTRESOURCE_H
#define PARSER_LISTRESOURCE_H

#include "ParentResource.h"

#include <memory>
#include <set>
#include <string>
#include <vector>

class ListResource: public ParentResource {
public:
  ListResource(std::string name, std::string module, std::string rest_endpoint,
               std::shared_ptr<ParentResource> parent,
               std::vector<PathParamField>&& fields);

  bool ValidateXPath(const std::string& xpath) const final;
};


#endif //PARSER_LISTRESOURCE_H
