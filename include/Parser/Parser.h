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

#include <libyang/libyang.h>

#include <cstring>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../Resources/Endpoint/Service.h"
#include "../Validators/Validator.h"

namespace polycube::polycubed::Rest::Parser {

using ValidatorList = std::vector<std::shared_ptr<Validators::Validator>>;
using ValidatorsType =
    std::pair<const ValidatorList, const std::unordered_set<std::type_index>>;

std::shared_ptr<Resources::Endpoint::Service> Parse(std::string &&yang);

std::string GetName(const std::string &yang);

const ValidatorList GetValidators(lys_type type);
}  // namespace polycube::polycubed::Rest::Parser
