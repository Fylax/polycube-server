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
#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <libyang/libyang.h>

#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <utility>
#include <unordered_set>
#include <vector>

#include "../Validators/Validator.h"
#include "../Validators/EnumValidator.h"
#include "../Validators/PatternValidator.h"
#include "../Resources/Cube.h"
#include "../Resources/LeafResource.h"
#include "../Resources/ParentResource.h"

using Pistache::Rest::Router;

using Validators = std::pair<
    const std::vector<std::shared_ptr<Validator>>,
    const std::unordered_set<std::type_index>
>;


namespace Parser {
std::shared_ptr<Cube> Parse(std::string&& yang);

std::string GetName(const std::string& yang);

const std::vector<std::shared_ptr<Validator>> getValidators(lys_type type);
}

#endif  // PARSER_PARSER_H
