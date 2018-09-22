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

#include <boost/lexical_cast.hpp>
#include <cstring>
#include <limits>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../../include/Validators/Validator.h"
#include "../../include/Validators/BitsValidator.h"
#include "../../include/Validators/EnumValidator.h"
#include "../../include/Validators/EmptyValidator.h"
#include "../../include/Validators/PatternValidator.h"
#include "../../include/Validators/LengthValidator.h"
#include "../../include/Validators/NumberValidators.h"
#include "../../include/Resources/Cube.h"
#include "../../include/Resources/LeafResource.h"
#include "../../include/Resources/ParentResource.h"

using Pistache::Rest::Router;
namespace Parser {
namespace {
ValidatorMap typedef_validators_[LY_DATA_TYPE_COUNT];

//  BEGIN DECLARATIONS
void ParseModule(const lys_module* module, const std::shared_ptr<Cube>& cube);

void ParseNode(lys_node* data, const std::shared_ptr<ParentResource>& parent);

void ParseGrouping(lys_node_grp* group,
                   const std::shared_ptr<ParentResource>& parent);

void ParseList(lys_node_list* list, std::shared_ptr<ParentResource> parent);

void ParseLeaf(lys_node_leaf* leaf, std::shared_ptr<ParentResource> parent);
// END DECLARATIONS

// BEGIN TYPE VALIDATORS
Validators ParseType(const char* name, lys_type type);

template<typename T>
std::unordered_map<T, T> ParseRange(std::string_view& range) {
  std::unordered_map<T, T> ranges;
  std::size_t pipe_pos = 0;
  do {
    // split string in '|' separated tokens
    pipe_pos = range.find('|');
    std::string_view current = range.substr(0, pipe_pos);
    if (pipe_pos != std::string_view::npos) {
      range = range.substr(pipe_pos + 1);
    }
    // split token in '..' separated ranges (if any)
    std::size_t range_pos = current.find("..");
    if (range_pos == std::string_view::npos) {  // exact value
      T exact = boost::lexical_cast<T>(current.data(), current.length());
      ranges.emplace(exact, exact);
    } else {  // ranges
      std::string_view min_ch = current.substr(0, range_pos);
      std::string_view max_ch = current.substr(range_pos + 2);
      T min = boost::lexical_cast<T>(min_ch.data(), min_ch.length());
      T max = boost::lexical_cast<T>(max_ch.data(), max_ch.length());
      ranges.emplace(min, max);
    }
  } while (pipe_pos != std::string_view::npos);
  return ranges;
}

std::shared_ptr<LengthValidator>
ParseLength(struct lys_restr* length, const bool binary) {
  auto validator = std::make_shared<LengthValidator>(binary);
  std::string_view expression{length->expr};
  validator->AddRanges(ParseRange<std::uint64_t>(expression));
  return validator;
}

Validators ParseEnum(const char* name, lys_type_info_enums enums) {
  auto validator = std::make_shared<EnumValidator>();
  for (unsigned i = 0; i < enums.count; ++i) {
    validator->AddEnum(enums.enm[i].name);
  }
  std::vector<std::shared_ptr<Validator>> validators = {
      std::static_pointer_cast<Validator>(validator)
  };
  auto map = ValidatorMap{{name, validators}};
  return std::make_unique<ValidatorMap>(map);
}

Validators ParseString(const char* name, lys_type_info_str str) {
  std::vector<std::shared_ptr<Validator>> validators;
  for (unsigned i = 0; i < str.pat_count; ++i) {
    auto current_pattern = str.patterns[i].expr;
    bool inverse = true;
    if (current_pattern[0] == 0x06) {
      inverse = false;
    }
    current_pattern = current_pattern + 1;
    validators.push_back(std::static_pointer_cast<Validator>(
        std::make_shared<PatternValidator>(current_pattern, inverse)));
  }
  if (str.length != nullptr) {
    validators.push_back(std::static_pointer_cast<Validator>(
        ParseLength(str.length, false)));
  }
  auto map = ValidatorMap{{name, validators}};
  return std::make_unique<ValidatorMap>(map);
}

template<typename T>
Validators ParseInteger(const char* name, lys_type_info_num num) {
  std::vector<std::shared_ptr<Validator>> validators;
  auto validator = std::make_shared<NumberValidator<T>>(
      std::numeric_limits<T>::min(),
      std::numeric_limits<T>::max());
  if (num.range != nullptr) {
    std::string_view expression{num.range->expr};
    validator->AddRanges(ParseRange<T>(expression));
  }
  auto map = ValidatorMap{{name, validators}};
  return std::make_unique<ValidatorMap>(map);
}

Validators ParseDecimal64(const char* name, lys_type_info_dec64 dec64) {
  std::vector<std::shared_ptr<Validator>> validators;
  auto validator = std::make_shared<DecimalValidator>(dec64.dig);
  if (dec64.range != nullptr) {
    std::string_view expression{dec64.range->expr};
    validator->AddRanges(ParseRange<Decimal64>(expression));
  }
  auto map = ValidatorMap{{name, validators}};
  return std::make_unique<ValidatorMap>(map);
}

Validators ParseBits(const char* name, lys_type_info_bits bits) {
  std::vector<std::shared_ptr<Validator>> validators;
  auto validator = std::make_shared<BitsValidator>();
  for (unsigned i = 0; i < bits.count; ++i) {
    auto bit = bits.bit[i];
    validator->AddBit(bit.pos, bit.name);
  }
  auto map = ValidatorMap{{name, validators}};
  return std::make_unique<ValidatorMap>(map);
}

Validators ParseBinary(const char* name, lys_type_info_binary binary) {
  std::vector<std::shared_ptr<Validator>> validators;
  if (binary.length != nullptr) {
    validators.push_back(std::static_pointer_cast<Validator>(
        ParseLength(binary.length, false)));
  }
  auto map = ValidatorMap{{name, validators}};
  return std::make_unique<ValidatorMap>(map);
}

Validators ParseLeafRef(const char* name, lys_type_info_lref lref) {
  return ParseType(name, lref.target->type);
}

Validators ParseEmpty(const char* name) {
  std::vector<std::shared_ptr<Validator>> validators{
      std::static_pointer_cast<Validator>(
          std::make_shared<EmptyValidator>()
      )
  };
  auto map = ValidatorMap{{name, validators}};
  return std::make_unique<ValidatorMap>(map);
}

Validators ParseType(const char* name, lys_type type) {
  switch (type.base) {
    case LY_TYPE_DER:
    case LY_TYPE_BINARY:
      return ParseBinary(name, type.info.binary);
    case LY_TYPE_BITS:
      return ParseBits(name, type.info.bits);
    case LY_TYPE_BOOL:
    case LY_TYPE_DEC64:
      return ParseDecimal64(name, type.info.dec64);
    case LY_TYPE_EMPTY:
      return ParseEmpty(name);
    case LY_TYPE_ENUM:
      return ParseEnum(name, type.info.enums);
    case LY_TYPE_IDENT:
      // TODO mississing (required identity first!)
    case LY_TYPE_INST:
    case LY_TYPE_LEAFREF:
      return ParseLeafRef(name, type.info.lref);
    case LY_TYPE_STRING:
      return ParseString(name, type.info.str);
    case LY_TYPE_UNION:
    case LY_TYPE_INT8:
      return ParseInteger<std::int8_t>(name, type.info.num);
    case LY_TYPE_UINT8:
      return ParseInteger<std::uint8_t>(name, type.info.num);
    case LY_TYPE_INT16:
      return ParseInteger<std::int16_t>(name, type.info.num);
    case LY_TYPE_UINT16:
      return ParseInteger<std::uint16_t>(name, type.info.num);
    case LY_TYPE_INT32:
      return ParseInteger<std::int32_t>(name, type.info.num);
    case LY_TYPE_UINT32:
      return ParseInteger<std::uint32_t>(name, type.info.num);
    case LY_TYPE_INT64:
      return ParseInteger<std::int64_t>(name, type.info.num);
    case LY_TYPE_UINT64:
      return ParseInteger<std::uint64_t>(name, type.info.num);
    default:
      throw std::runtime_error("Unsupported Type");
  }
}

const std::vector<std::shared_ptr<Validator>>
GetValidators(lys_type type) {
  bool isDerived = (type.der->type.der != nullptr);
  if (!isDerived) {
    auto inplace = ParseType("", type);
    if (inplace->count("") == 1) {
      return inplace->at("");
    }
    return std::vector<std::shared_ptr<Validator>>();
  }
  if (typedef_validators_[type.base].count(type.der->name) == 1) {
    auto validators = typedef_validators_[type.base].at(type.der->name);
    // move out a copy of the original vector
    return std::vector<std::shared_ptr<Validator>>(validators);
  }
  return std::vector<std::shared_ptr<Validator>>();
}
// END TYPE VALIDATORS

void ParseModule(const lys_module* module, const std::shared_ptr<Cube>& cube) {
  auto typedefs = module->tpdf;
  for (auto i = 0; i < module->tpdf_size; ++i) {
    auto current_typedef = typedefs[i];
    auto validators = ParseType(current_typedef.name, current_typedef.type);
    typedef_validators_[current_typedef.type.base].insert(
        validators->begin(), validators->end());
  }
  auto data = module->data;
  while (data) {
    ParseNode(data, std::static_pointer_cast<ParentResource>(cube));
    data = data->next;
  }
}

void ParseNode(lys_node* data, const std::shared_ptr<ParentResource>& parent) {
  if (!data) return;
  switch (data->nodetype) {
    case LYS_UNKNOWN:
      break;
    case LYS_CONTAINER:
      break;
    case LYS_CHOICE:
      break;
    case LYS_LEAF:
      ParseLeaf(reinterpret_cast<lys_node_leaf*>(data), parent);
      break;
    case LYS_LEAFLIST:
      break;
    case LYS_LIST:
      ParseList(reinterpret_cast<lys_node_list*>(data), parent);
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
      ParseGrouping(reinterpret_cast<lys_node_grp*>(data), parent);
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

void ParseGrouping(lys_node_grp* group,
                   const std::shared_ptr<ParentResource>& parent) {
  auto child = group->child;
  while (child) {
    ParseNode(child, parent);
    child = child->next;
  }
}

void ParseList(lys_node_list* list, std::shared_ptr<ParentResource> parent) {
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
      auto validator = GetValidators(key->type);
      keys.emplace_back(child->name, std::move(validator));
    }
    child = child->next;
  }

  auto resource = std::make_shared<ParentResource>(list->name, rest_endpoint,
                                                   parent, std::move(keys));
  // get all children
  child = list->child;
  while (child != nullptr) {
    if (key_names.count(child->name) == 0) {
      ParseNode(child, resource);
    }
    child = child->next;
  }
  parent->AddChild(resource);
}

void ParseLeaf(lys_node_leaf* leaf, std::shared_ptr<ParentResource> parent) {
  bool configurable = ((leaf->flags & LYS_CONFIG_MASK) ^ 2) != 0;
  bool mandatory = (leaf->flags & LYS_MAND_MASK) != 0;
  auto validators = GetValidators(leaf->type);
  auto field = std::make_unique<JsonBodyField>(
      std::move(validators), JsonBodyField::FromYangType(leaf->type.base));
  std::unique_ptr<const std::string> default_value = nullptr;
  if (leaf->dflt != nullptr) {
    default_value = std::make_unique<const std::string>(leaf->dflt);
  }
  auto leaf_res = std::make_unique<LeafResource>(
      leaf->name, parent->Endpoint() + leaf->name, parent,
      std::move(field), configurable, mandatory, std::move(default_value));
  parent->AddChild(std::move(leaf_res));
}

}  // namespace

std::string GetName(const std::string& yang) {
  auto context = ly_ctx_new("/home/nico/dev/iovnet/services/resources/", 0);
  if (!context) {
    throw std::runtime_error("cannot create new context");
  }

  auto module = lys_parse_mem(context, yang.data(), LYS_IN_YANG);
  if (!module) {
    throw std::invalid_argument("invalid yang data");
  }
  return std::string{module->name};
}

std::shared_ptr<Cube> Parse(std::string&& yang) {
  auto context = ly_ctx_new("/home/nico/dev/iovnet/services/resources/", 0);
  if (!context) {
    throw std::runtime_error("cannot create new context");
  }

  auto module = lys_parse_mem(context, yang.data(), LYS_IN_YANG);

  if (!module) {
    throw std::invalid_argument("invalid yang data");
  }

  auto cube = std::make_shared<Cube>(module->name, std::string{'/'});
  for (auto i = 0; i < module->imp_size; ++i) {
    ParseModule(module->imp[i].module, cube);
  }
  ParseModule(module, cube);
  typedef_validators_->clear();
  return cube;
}
}
