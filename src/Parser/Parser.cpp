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
#include "../../include/Resources/LeafResource.h"
#include "../../include/Resources/ParentResource.h"

using Pistache::Rest::Router;

namespace {
ValidatorMap perTypeValidators[LY_DATA_TYPE_COUNT];
}

const std::vector<std::shared_ptr<Validator>>
getValidators(lys_type type) {
  bool isDerived = (type.der->type.der != nullptr);
  if (!isDerived) {
    auto inplace = parseType("", type);
    if (inplace->count("") == 1) {
      return inplace->at("");
    }
    return std::vector<std::shared_ptr<Validator>>();
  }
  if (perTypeValidators[type.base].count(type.der->name) == 1) {
    auto validators = perTypeValidators[type.base].at(type.der->name);
    // move out a copy of the original vector
    return std::vector<std::shared_ptr<Validator>>(validators);
  }
  return std::vector<std::shared_ptr<Validator>>();
}

void
parseModule(const lys_module* module, ParentResource& parent,
            std::shared_ptr<Router> router) {
  auto typedefs = module->tpdf;
  for (auto i = 0; i < module->tpdf_size; ++i) {
    auto current_typedef = typedefs[i];
    auto validators = parseType(current_typedef.name, current_typedef.type);
    perTypeValidators[current_typedef.type.base].insert(
        validators->begin(), validators->end());
  }
  auto data = module->data;
  while (data) {
    parseNode(data, parent, router);
    data = data->next;
  }
}

Validators parseType(const char* name, lys_type type) {
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

Validators parseEnum(const char* name, lys_type_info_enums enums) {
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

Validators parseString(const char* name, lys_type_info_str str) {
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

void parseNode(lys_node* data, ParentResource& parent,
               std::shared_ptr<Router> router) {
  if (!data) return;
  switch (data->nodetype) {
    case LYS_UNKNOWN:
      break;
    case LYS_CONTAINER:
      break;
    case LYS_CHOICE:
      break;
    case LYS_LEAF:
      parseLeaf(reinterpret_cast<lys_node_leaf*>(data), parent, router);
      break;
    case LYS_LEAFLIST:
      break;
    case LYS_LIST:
      parseList(reinterpret_cast<lys_node_list*>(data), parent, router);
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
      parseGrouping(reinterpret_cast<lys_node_grp*>(data), parent, router);
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

void
parseGrouping(lys_node_grp* group, ParentResource& parent,
              std::shared_ptr<Router> router) {
  auto child = group->child;
  while (child) {
    parseNode(child, parent, router);
    child = child->next;
  }
}

void parseList(lys_node_list* list, ParentResource& parent,
               std::shared_ptr<Router> router) {
  auto keys = std::vector<PathParamField>();
  auto key_names = std::set<std::string>();
  auto rest_endpoint = parent.Endpoint() + list->name + '/';
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
      keys.emplace_back(std::string{':'} + child->name, std::move(validator));
    }
    child = child->next;
  }

  auto resource = ParentResource(list->name, router,
                                 rest_endpoint,
                                 std::make_shared<ParentResource>(parent),
                                 std::move(keys));
  // get all children
  child = list->child;
  while (child != nullptr) {
    if (key_names.count(child->name) == 0) {
      parseNode(child, resource, router);
    }
    child = child->next;
  }
  parent.AddChild(std::make_unique<ParentResource>(std::move(resource)));
}

void parseLeaf(lys_node_leaf* leaf, ParentResource& parent,
               std::shared_ptr<Router> router) {
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
      leaf->name, router, parent.Endpoint() + ':' + leaf->name,
      std::make_shared<ParentResource>(parent), std::move(field), configurable,
      mandatory, std::move(default_value));
  parent.AddChild(std::move(leaf_res));
}
