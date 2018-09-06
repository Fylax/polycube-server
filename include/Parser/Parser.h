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
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../Validators/Validator.h"
#include "../Validators/EnumValidator.h"
#include "../Validators/PatternValidator.h"
#include "../Resources/LeafResource.h"
#include "../Resources/ParentResource.h"

using ValidatorMap = std::unordered_map<
    std::string, const std::vector<std::shared_ptr<Validator>>
>;

using Validators = std::unique_ptr<ValidatorMap>;

const std::vector<std::shared_ptr<Validator>> getValidators(lys_type type);

void parseModule(const lys_module* module, std::shared_ptr<ParentResource> parent);

Validators parseType(const char* name, lys_type type);

Validators parseEnum(const char* name, lys_type_info_enums enums);

Validators parseString(const char* name, lys_type_info_str str);

void parseNode(lys_node* data, std::shared_ptr<ParentResource> parent);

void parseGrouping(lys_node_grp* group, std::shared_ptr<ParentResource> parent);

void parseLeaf(lys_node_leaf* leaf, std::shared_ptr<ParentResource> parent);


#endif //PARSER_PARSER_H
