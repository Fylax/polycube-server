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
#include "../../include/Parser/Parser.h"

#include <libyang/libyang.h>

#include <cstring>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../../include/Validators/Validator.h"
#include "../../include/Validators/EnumValidator.h"
#include "../../include/Validators/PatternValidator.h"
#include "../../include/Resources/Cube.h"
#include "../../include/Resources/LeafResource.h"
#include "../../include/Resources/ParentResource.h"

using Pistache::Rest::Router;

Parser::Parser(std::string&& yang) {
  auto context = ly_ctx_new("/home/nico/dev/iovnet/services/resources/", 0);
  if (!context) {
    throw std::runtime_error("cannot create new context");
  }

  auto module = lys_parse_mem(context, yang.c_str(), LYS_IN_YANG);

  if (!module) {
    throw std::invalid_argument("invalid yang data");
  }

  auto cube = std::make_shared<Cube>(module->name,
                   std::string{'/'} + module->name + "/:cube_name/");
  for (auto i = 0; i < module->imp_size; ++i) {
    parseModule(module->imp[i].module, cube);
  }
  parseModule(module, cube);
}

const std::vector<std::shared_ptr<Validator>>
Parser::getValidators(lys_type type) {
  bool isDerived = (type.der->type.der != nullptr);
  if (!isDerived) {
    auto inplace = parseType("", type);
    if (inplace->count("") == 1) {
      return inplace->at("");
    }
    return std::vector<std::shared_ptr<Validator>>();
  }
  if (typedef_validators[type.base].count(type.der->name) == 1) {
    auto validators = typedef_validators[type.base].at(type.der->name);
    // move out a copy of the original vector
    return std::vector<std::shared_ptr<Validator>>(validators);
  }
  return std::vector<std::shared_ptr<Validator>>();
}

void Parser::parseModule(const lys_module* module, std::shared_ptr<Cube> cube) {
  auto typedefs = module->tpdf;
  for (auto i = 0; i < module->tpdf_size; ++i) {
    auto current_typedef = typedefs[i];
    auto validators = parseType(current_typedef.name, current_typedef.type);
    typedef_validators[current_typedef.type.base].insert(
        validators->begin(), validators->end());
  }
  auto data = module->data;
  while (data) {
    parseNode(data, std::static_pointer_cast<ParentResource>(cube));
    data = data->next;
  }
}

Validators Parser::parseType(const char* name, lys_type type) {
  switch (type.base) {
    case LY_TYPE_DER:
    case LY_TYPE_BINARY:
    case LY_TYPE_BITS:
    case LY_TYPE_BOOL:
    case LY_TYPE_DEC64:
    case LY_TYPE_EMPTY:
    case LY_TYPE_ENUM:
      return parseEnum(name, type.info.enums);
    case LY_TYPE_IDENT:
    case LY_TYPE_INST:
    case LY_TYPE_LEAFREF:
    case LY_TYPE_STRING:
      return parseString(name, type.info.str);
    case LY_TYPE_UNION:
    case LY_TYPE_INT8:
    case LY_TYPE_UINT8:
    case LY_TYPE_INT16:
    case LY_TYPE_UINT16:
    case LY_TYPE_INT32:
    case LY_TYPE_UINT32:
    case LY_TYPE_INT64:
    case LY_TYPE_UINT64:
    default:
      throw std::runtime_error("Unsupported Type");
  }
}

Validators Parser::parseEnum(const char* name, lys_type_info_enums enums) {
  std::unordered_set<std::string> allowed;
  for (unsigned i = 0; i < enums.count; ++i) {
    allowed.insert(enums.enm[i].name);
  }
  std::vector<std::shared_ptr<Validator>> validators = {
      std::make_shared<EnumValidator>(allowed)
  };
  auto map = ValidatorMap{{name, validators}};
  return std::make_unique<ValidatorMap>(map);
}

Validators Parser::parseString(const char* name, lys_type_info_str str) {
  std::vector<std::shared_ptr<Validator>> validators;
  for (unsigned i = 0; i < str.pat_count; ++i) {
    auto current_pattern = str.patterns[i].expr;
    // check match byte
    if (current_pattern[0] == 0x06) {
      // skip first byte
      current_pattern = current_pattern + 1;
    } else {
      throw std::runtime_error("unsupported NACK patterns");
    }
    validators.push_back(std::static_pointer_cast<Validator>(
        std::make_shared<PatternValidator>(current_pattern))
    );
  }
  auto map = ValidatorMap{{name, validators}};
  return std::make_unique<ValidatorMap>(map);
}

void Parser::parseNode(lys_node* data, std::shared_ptr<ParentResource> parent) {
  if (!data) return;
  switch (data->nodetype) {
    case LYS_UNKNOWN:
      break;
    case LYS_CONTAINER:
      break;
    case LYS_CHOICE:
      break;
    case LYS_LEAF:
      parseLeaf(reinterpret_cast<lys_node_leaf*>(data), parent);
      break;
    case LYS_LEAFLIST:
      break;
    case LYS_LIST:
      parseList(reinterpret_cast<lys_node_list*>(data), parent);
      break;
    case LYS_ANYXML:
      break;
    case LYS_CASE:
      break;
    case LYS_NOTIF:
      break;
    case LYS_RPC:
      break;
    case LYS_INPUT:
      break;
    case LYS_OUTPUT:
      break;
    case LYS_GROUPING:
      parseGrouping(reinterpret_cast<lys_node_grp*>(data), parent);
      break;
    case LYS_USES:
      break;
    case LYS_AUGMENT:
      break;
    case LYS_ACTION:
      break;
    case LYS_ANYDATA:
      break;
    case LYS_EXT:
      break;
  }
}

void Parser::parseGrouping(lys_node_grp* group, std::shared_ptr<ParentResource> parent) {
  auto child = group->child;
  while (child) {
    parseNode(child, parent);
    child = child->next;
  }
}

void Parser::parseList(lys_node_list* list, std::shared_ptr<ParentResource> parent) {
  auto keys = std::vector<PathParamField>();
  auto key_names = std::set<std::string>();
  auto rest_endpoint = parent->Endpoint() + list->name + '/';
  if (list->keys_size != 0) {
    keys.reserve(list->keys_size);
    std::string item;
    auto stream = std::stringstream(list->keys_str);
    while (std::getline(stream, item, ' ')) {
      rest_endpoint += ':' + item + '/';
      key_names.insert(item);
    }
  }
  // get all keys
  auto child = list->child;
  while (child != nullptr) {
    if (key_names.count(child->name) != 0) {
      auto key = reinterpret_cast<lys_node_leaf*>(child);
      auto validator = getValidators(key->type);
      keys.emplace_back(std::string {':'} + child->name, std::move(validator));
    }
    child = child->next;
  }

  auto resource = std::make_shared<ParentResource>(list->name, rest_endpoint,
      parent, std::move(keys));
  // get all children
  child = list->child;
  while (child != nullptr) {
    if (key_names.count(child->name) == 0) {
      parseNode(child, resource);
    }
    child = child->next;
  }
  parent->AddChild(resource);
}

void Parser::parseLeaf(lys_node_leaf* leaf, std::shared_ptr<ParentResource> parent) {
  bool configurable = ((leaf->flags & LYS_CONFIG_MASK) ^ 2) != 0;
  bool mandatory = (leaf->flags & LYS_MAND_MASK) != 0;
  auto validators = getValidators(leaf->type);
  auto field = std::make_unique<JsonBodyField>(
      std::move(validators), JsonBodyField::FromYangType(leaf->type.base));
  std::unique_ptr<const std::string> default_value = nullptr;
  if (leaf->dflt != nullptr) {
    default_value = std::make_unique<const std::string>(leaf->dflt);
  }
  auto leaf_res = std::make_unique<LeafResource>(
      leaf->name, parent->Endpoint() + ':' + leaf->name, parent,
      std::move(field), configurable, mandatory, std::move(default_value));
  parent->AddChild(std::move(leaf_res));
}

