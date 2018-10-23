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

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "../../include/Resources/Endpoint/ChoiceResource.h"
#include "../../include/Resources/Endpoint/LeafListResource.h"
#include "../../include/Resources/Endpoint/LeafResource.h"
#include "../../include/Resources/Endpoint/ListResource.h"
#include "../../include/Resources/Endpoint/ParentResource.h"
#include "../../include/Resources/Endpoint/PathParamField.h"

#include "../../include/Types/Decimal64.h"
#include "../../include/Types/Dummies.h"

#include "../../include/Resources/Endpoint/CaseResource.h"
#include "../../include/Validators/BitsValidator.h"
#include "../../include/Validators/BoolValidator.h"
#include "../../include/Validators/EmptyValidator.h"
#include "../../include/Validators/EnumValidator.h"
#include "../../include/Validators/LengthValidator.h"
#include "../../include/Validators/NumberValidators.h"
#include "../../include/Validators/PatternValidator.h"
#include "../../include/Validators/UnionValidator.h"
#include "../../include/Validators/XPathValidator.h"

namespace polycube::polycubed::Rest::Parser {
namespace {
/** Stores typedef name and associated list of Validators */
std::unordered_map<std::string, ValidatorList>
    typedef_validators_[LY_DATA_TYPE_COUNT];

//  BEGIN DECLARATIONS
void ParseModule(const lys_module *module,
                 const std::shared_ptr<Resources::Endpoint::Service> &cube);

void ParseNode(const lys_node *data,
               const std::shared_ptr<Resources::Body::ParentResource> &parent,
               bool generate_endpoint);

void ParseContainer(
    const lys_node_container *data,
    const std::shared_ptr<Resources::Body::ParentResource> &parent,
    bool generate_endpoint);

void ParseGrouping(
    const lys_node_grp *group,
    const std::shared_ptr<Resources::Body::ParentResource> &parent,
    bool generate_endpoint);

void ParseList(const lys_node_list *list,
               const std::shared_ptr<Resources::Body::ParentResource> &parent,
               bool generate_endpoint);

void ParseLeaf(const lys_node_leaf *leaf,
               const std::shared_ptr<Resources::Body::ParentResource> &parent,
               bool generate_endpoint);

void ParseLeafList(
    const lys_node_leaflist *leaflist,
    const std::shared_ptr<Resources::Body::ParentResource> &parent,
    bool generate_endpoint);

void ParseChoice(const lys_node_choice *choice,
                 const std::shared_ptr<Resources::Body::ParentResource> &parent,
                 bool generate_endpoint);

void ParseCase(const lys_node_case *case_node,
               const std::shared_ptr<Resources::Body::ParentResource> &parent,
               bool generate_endpoint);

void ParseAny(const lys_node *data,
              const std::shared_ptr<Resources::Body::ParentResource> &parent,
              bool generate_endpoint);

void ParseRpcAction(
    const lys_node_rpc_action *data,
    const std::shared_ptr<Resources::Body::ParentResource> &parent);
// END DECLARATIONS

// BEGIN TYPE VALIDATORS
ValidatorsType ParseType(lys_type type);

template <typename T>
std::unordered_map<T, T> ParseRange(std::string_view &range) {
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

std::shared_ptr<Validators::LengthValidator> ParseLength(
    const lys_restr *length, const bool binary) {
  auto validator = std::make_shared<Validators::LengthValidator>(binary);
  std::string_view expression{length->expr};
  validator->AddRanges(ParseRange<std::uint64_t>(expression));
  return validator;
}

ValidatorsType ParseEnum(const lys_type_info_enums enums) {
  auto validator = std::make_shared<Validators::EnumValidator>();
  for (unsigned i = 0; i < enums.count; ++i) {
    validator->AddEnum(enums.enm[i].name);
  }
  ValidatorList validators = {
      std::static_pointer_cast<Validators::Validator>(validator)};
  return {std::move(validators), {std::type_index(typeid(Types::Enum))}};
}

ValidatorsType ParseString(const lys_type_info_str str) {
  ValidatorList validators;
  for (unsigned i = 0; i < str.pat_count; ++i) {
    auto current_pattern = str.patterns[i].expr;
    bool inverse = true;
    if (current_pattern[0] == 0x06) {
      inverse = false;
    }
    current_pattern = current_pattern + 1;
    validators.push_back(std::static_pointer_cast<Validators::Validator>(
        std::make_shared<Validators::PatternValidator>(current_pattern,
                                                       inverse)));
  }
  if (str.length != nullptr) {
    validators.push_back(std::static_pointer_cast<Validators::Validator>(
        ParseLength(str.length, false)));
  }
  return {std::move(validators), {std::type_index(typeid(std::string))}};
}

template <typename T>
ValidatorsType ParseInteger(const lys_type_info_num num) {
  ValidatorList validators;
  auto validator = std::make_shared<Validators::NumberValidator<T>>(
      std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
  if (num.range != nullptr) {
    std::string_view expression{num.range->expr};
    validator->AddRanges(ParseRange<T>(expression));
  }
  return {std::move(validators), {std::type_index(typeid(T))}};
}

ValidatorsType ParseDecimal64(const lys_type_info_dec64 dec64) {
  ValidatorList validators;
  auto validator = std::make_shared<Validators::DecimalValidator>(dec64.dig);
  if (dec64.range != nullptr) {
    std::string_view expression{dec64.range->expr};
    validator->AddRanges(ParseRange<Types::Decimal64>(expression));
  }
  return {std::move(validators), {std::type_index(typeid(Types::Decimal64))}};
}

ValidatorsType ParseBits(const lys_type_info_bits bits) {
  ValidatorList validators;
  auto validator = std::make_shared<Validators::BitsValidator>();
  for (unsigned i = 0; i < bits.count; ++i) {
    auto bit = bits.bit[i];
    validator->AddBit(bit.pos, bit.name);
  }
  return {std::move(validators), {std::type_index(typeid(Types::Bits))}};
}

ValidatorsType ParseBinary(const lys_type_info_binary binary) {
  ValidatorList validators;
  if (binary.length != nullptr) {
    validators.push_back(std::static_pointer_cast<Validators::Validator>(
        ParseLength(binary.length, false)));
  }
  return {std::move(validators), {std::type_index(typeid(Types::Binary))}};
}

ValidatorsType ParseLeafRef(const lys_type_info_lref lref) {
  // TODO lref contains info about require-instance, missing ATM
  return ParseType(lref.target->type);
}

ValidatorsType ParseBoolean() {
  auto validator = std::make_shared<Validators::BoolValidator>();
  ValidatorList validators{
      std::static_pointer_cast<Validators::Validator>(validator)};
  return {std::move(validators), {std::type_index(typeid(bool))}};
}

ValidatorsType ParseEmpty() {
  ValidatorList validators{std::static_pointer_cast<Validators::Validator>(
      std::make_shared<Validators::EmptyValidator>())};
  return {std::move(validators), {std::type_index(typeid(Types::Empty))}};
}

ValidatorsType ParseUnion(const lys_type_info_union yunion) {
  ValidatorList validators;
  std::unordered_set<std::type_index> types;
  auto validator = std::make_shared<Validators::UnionValidator>();
  for (unsigned i = 0; i < yunion.count; ++i) {
    const auto &parsed = ParseType(yunion.types[i]);
    types.insert(std::begin(parsed.second), std::end(parsed.second));
    for (const auto &type : parsed.second) {
      validator->AddType(type, parsed.first);
    }
  }
  return {std::move(validators), std::move(types)};
}

ValidatorsType ParseInstanceIdentifier(
    [[maybe_unused]] const lys_type_info_inst iid, const char *context) {
  // TODO iid contains info about require-instance, missing ATM
  ValidatorList validators{std::static_pointer_cast<Validators::Validator>(
      std::make_shared<Validators::XPathValidator>(context))};
  return {std::move(validators), {std::type_index(typeid(std::string))}};
}

ValidatorsType ParseType(const lys_type type) {
  switch (type.base) {
  case LY_TYPE_BINARY:
    return ParseBinary(type.info.binary);
  case LY_TYPE_BITS:
    return ParseBits(type.info.bits);
  case LY_TYPE_BOOL:
    return ParseBoolean();
  case LY_TYPE_DEC64:
    return ParseDecimal64(type.info.dec64);
  case LY_TYPE_EMPTY:
    return ParseEmpty();
  case LY_TYPE_ENUM:
    return ParseEnum(type.info.enums);
  case LY_TYPE_IDENT:
    // TODO mississing (required identity first!)
  case LY_TYPE_INST:
    return ParseInstanceIdentifier(type.info.inst, type.parent->module->name);
  case LY_TYPE_LEAFREF:
    return ParseLeafRef(type.info.lref);
  case LY_TYPE_STRING:
    return ParseString(type.info.str);
  case LY_TYPE_UNION:
    return ParseUnion(type.info.uni);
  case LY_TYPE_INT8:
    return ParseInteger<std::int8_t>(type.info.num);
  case LY_TYPE_UINT8:
    return ParseInteger<std::uint8_t>(type.info.num);
  case LY_TYPE_INT16:
    return ParseInteger<std::int16_t>(type.info.num);
  case LY_TYPE_UINT16:
    return ParseInteger<std::uint16_t>(type.info.num);
  case LY_TYPE_INT32:
    return ParseInteger<std::int32_t>(type.info.num);
  case LY_TYPE_UINT32:
    return ParseInteger<std::uint32_t>(type.info.num);
  case LY_TYPE_INT64:
    return ParseInteger<std::int64_t>(type.info.num);
  case LY_TYPE_UINT64:
    return ParseInteger<std::uint64_t>(type.info.num);
  default:
    throw std::runtime_error("Unsupported Type");
  }
}

const ValidatorList GetValidators(const lys_type type) {
  bool is_derived = (type.der->type.der != nullptr);
  if (!is_derived) {
    return ParseType(type).first;
  }
  if (typedef_validators_[type.base].count(type.der->name) == 1) {
    const auto &validators = typedef_validators_[type.base].at(type.der->name);
    // move out a copy of the original vector
    return ValidatorList(validators);
  }
  // if it not derived and it is not a typedef, it is a type
  // without restrictions, thus an empty vector is returned
  return ValidatorList();
}
// END TYPE VALIDATORS

// START YANG NODES PARSING
void ParseModule(const lys_module *module,
                 const std::shared_ptr<Resources::Endpoint::Service> &cube) {
  auto typedefs = module->tpdf;
  for (auto i = 0; i < module->tpdf_size; ++i) {
    const auto &current_typedef = typedefs[i];
    const auto &validators = ParseType(current_typedef.type);
    typedef_validators_[current_typedef.type.base].emplace(current_typedef.name,
                                                           validators.first);
  }
  auto data = module->data;
  while (data) {
    ParseNode(data,
              std::static_pointer_cast<Resources::Body::ParentResource>(cube),
              true);
    data = data->next;
  }
}

void ParseNode(const lys_node *data,
               const std::shared_ptr<Resources::Body::ParentResource> &parent,
               bool generate_endpoint) {
  switch (data->nodetype) {
  case LYS_UNKNOWN:
    break;
  case LYS_CONTAINER:
    ParseContainer(reinterpret_cast<const lys_node_container *>(data), parent,
                   generate_endpoint);
    break;
  case LYS_CHOICE:
    ParseChoice(reinterpret_cast<const lys_node_choice *>(data), parent,
                generate_endpoint);
    break;
  case LYS_LEAF:
    ParseLeaf(reinterpret_cast<const lys_node_leaf *>(data), parent,
              generate_endpoint);
    break;
  case LYS_LEAFLIST:
    ParseLeafList(reinterpret_cast<const lys_node_leaflist *>(data), parent,
                  generate_endpoint);
    break;
  case LYS_LIST:
    ParseList(reinterpret_cast<const lys_node_list *>(data), parent,
              generate_endpoint);
    break;
  case LYS_ANYXML:
    ParseAny(data, parent, generate_endpoint);
    break;
  case LYS_CASE:
    ParseCase(reinterpret_cast<const lys_node_case *>(data), parent,
              generate_endpoint);
    break;
  case LYS_NOTIF:
    break;
  case LYS_RPC:
    throw std::invalid_argument("RPC not supported");
    break;
  case LYS_INPUT:
    ParseNode(data->child, parent, false);
    break;
  case LYS_OUTPUT:
    break;
  case LYS_GROUPING:
    ParseGrouping(reinterpret_cast<const lys_node_grp *>(data), parent,
                  generate_endpoint);
    break;
  case LYS_USES:
    break;
  case LYS_AUGMENT:
    break;
  case LYS_ACTION:
    ParseRpcAction(reinterpret_cast<const lys_node_rpc_action *>(data), parent);
    break;
  case LYS_ANYDATA:
    ParseAny(data, parent, generate_endpoint);
    break;
  case LYS_EXT:
    break;
  }
}

void ParseContainer(
    const lys_node_container *data,
    const std::shared_ptr<Resources::Body::ParentResource> &parent,
    bool generate_endpoint) {
  std::shared_ptr<Resources::Body::ParentResource> resource;
  if (generate_endpoint) {
    auto e_parent =
        std::dynamic_pointer_cast<Resources::Endpoint::ParentResource>(parent);
    auto endpoint = e_parent->Endpoint() + data->name + '/';
    resource = std::make_shared<Resources::Endpoint::ParentResource>(
        data->name, data->module->name, endpoint, e_parent,
        data->presence != nullptr);
  } else {
    resource = std::make_shared<Resources::Body::ParentResource>(
        data->name, data->module->name, parent, data->presence != nullptr);
  }
  auto child = data->child;
  while (child != nullptr) {
    ParseNode(child, resource, generate_endpoint);
    child = child->next;
  }
  parent->AddChild(resource);
}

void ParseGrouping(
    const lys_node_grp *group,
    const std::shared_ptr<Resources::Body::ParentResource> &parent,
    bool generate_endpoint) {
  auto child = group->child;
  while (child) {
    ParseNode(child, parent, generate_endpoint);
    child = child->next;
  }
}

void ParseList(const lys_node_list *list,
               const std::shared_ptr<Resources::Body::ParentResource> &parent,
               bool generate_endpoint) {
  std::vector<std::pair<std::string,
                        std::vector<std::shared_ptr<Validators::Validator>>>>
      keys{};
  auto key_names = std::set<std::string>();
  std::string rest_endpoint;
  std::string rest_endpoint_multiple;
  if (generate_endpoint) {
    rest_endpoint_multiple =
        std::dynamic_pointer_cast<Resources::Endpoint::ParentResource>(parent)
            ->Endpoint() +
        list->name + '/';
    rest_endpoint = rest_endpoint_multiple;
  }
  if (list->keys_size != 0) {
    keys.reserve(list->keys_size);
    std::string item;
    auto stream = std::stringstream(list->keys_str);
    while (std::getline(stream, item, ' ')) {
      if (generate_endpoint) {
        rest_endpoint += ':' + item + '/';
      }
      key_names.insert(item);
    }
  }
  // get all keys
  auto child = list->child;
  while (child != nullptr) {
    if (key_names.count(child->name) != 0) {
      const auto &key = reinterpret_cast<lys_node_leaf *>(child);
      auto validator = GetValidators(key->type);
      keys.emplace_back(child->name, std::move(validator));
    }
    child = child->next;
  }

  std::shared_ptr<Resources::Body::ListResource> resource;
  if (generate_endpoint) {
    auto e_parent =
        std::dynamic_pointer_cast<Resources::Endpoint::ParentResource>(parent);
    resource = std::make_shared<Resources::Endpoint::ListResource>(
        list->name, list->module->name, e_parent, rest_endpoint,
        rest_endpoint_multiple, std::move(keys));
  } else {
    resource = std::make_shared<Resources::Body::ListResource>(
        list->name, list->module->name, parent, std::move(keys));
  }
  // parse each child using the generic "node" parsing function
  child = list->child;
  while (child != nullptr) {
    if (key_names.count(child->name) == 0) {
      ParseNode(child, resource, generate_endpoint);
    }
    child = child->next;
  }
  parent->AddChild(resource);
}

void ParseLeaf(const lys_node_leaf *leaf,
               const std::shared_ptr<Resources::Body::ParentResource> &parent,
               bool generate_endpoint) {
  bool configuration = ((leaf->flags & LYS_CONFIG_MASK) ^ 2) != 0;
  bool mandatory = (leaf->flags & LYS_MAND_MASK) != 0;
  auto validators = GetValidators(leaf->type);
  auto field = std::make_unique<Resources::Body::JsonBodyField>(
      leaf->type.base, std::move(validators));
  std::unique_ptr<const std::string> default_value = nullptr;
  if (leaf->dflt != nullptr) {
    default_value = std::make_unique<const std::string>(leaf->dflt);
  }
  std::unique_ptr<Resources::Body::LeafResource> leaf_res;
  if (generate_endpoint) {
    auto e_parent =
        std::dynamic_pointer_cast<Resources::Endpoint::ParentResource>(parent);
    auto endpoint = e_parent->Endpoint() + leaf->name;
    leaf_res = std::make_unique<Resources::Endpoint::LeafResource>(
        leaf->name, leaf->module->name, endpoint, e_parent, std::move(field),
        configuration, mandatory, std::move(default_value));
  } else {
    leaf_res = std::make_unique<Resources::Body::LeafResource>(
        leaf->name, leaf->module->name, parent, std::move(field), configuration,
        mandatory, std::move(default_value));
  }
  parent->AddChild(std::move(leaf_res));
}

void ParseLeafList(
    const lys_node_leaflist *leaflist,
    const std::shared_ptr<Resources::Body::ParentResource> &parent,
    bool generate_endpoint) {
  bool configuration = ((leaflist->flags & LYS_CONFIG_MASK) ^ 2) != 0;
  bool mandatory = (leaflist->flags & LYS_MAND_MASK) != 0;
  auto validators = GetValidators(leaflist->type);
  auto field = std::make_unique<Resources::Body::JsonBodyField>(
      leaflist->type.base, std::move(validators));
  std::vector<std::string> default_value;
  if (leaflist->dflt != nullptr) {
    const auto defaults = leaflist->dflt;
    for (std::uint8_t i = 0; i < leaflist->dflt_size; ++i) {
      default_value.emplace_back(defaults[i]);
    }
  }
  std::shared_ptr<Resources::Body::LeafListResource> resource;
  if (generate_endpoint) {
    auto e_parent =
        std::dynamic_pointer_cast<Resources::Endpoint::ParentResource>(parent);
    auto endpoint = e_parent->Endpoint() + leaflist->name;
    resource = std::make_unique<Resources::Endpoint::LeafListResource>(
        leaflist->name, leaflist->module->name, endpoint, e_parent,
        std::move(field), configuration, mandatory, std::move(default_value));
  } else {
    resource = std::make_unique<Resources::Body::LeafListResource>(
        leaflist->name, leaflist->module->name, parent, std::move(field),
        configuration, mandatory, std::move(default_value));
  }
  parent->AddChild(std::move(resource));
}

void ParseChoice(const lys_node_choice *choice,
                 const std::shared_ptr<Resources::Body::ParentResource> &parent,
                 bool generate_endpoint) {
  bool mandatory = (choice->flags & LYS_MAND_MASK) != 0;
  std::unique_ptr<const std::string> default_case = nullptr;
  if (choice->dflt != nullptr) {
      default_case = std::make_unique<const std::string>(choice->dflt->name);
  }
  std::shared_ptr<Resources::Body::ChoiceResource> resource;
  if (generate_endpoint) {
    auto e_parent =
        std::dynamic_pointer_cast<Resources::Endpoint::ParentResource>(parent);
    resource = std::make_shared<Resources::Endpoint::ChoiceResource>(
        choice->name, choice->module->name, e_parent, mandatory,
        std::move(default_case));
  } else {
    resource = std::make_shared<Resources::Body::ChoiceResource>(
        choice->name, choice->module->name, parent, mandatory,
        std::move(default_case));
  }
  auto child = choice->child;
  while (child != nullptr) {
    ParseNode(child, resource, generate_endpoint);
    child = child->next;
  }
  parent->AddChild(std::move(resource));
}

void ParseCase(const lys_node_case *case_node,
               const std::shared_ptr<Resources::Body::ParentResource> &parent,
               bool generate_endpoint) {
  std::shared_ptr<Resources::Body::CaseResource> resource;
  if (generate_endpoint) {
    auto e_parent =
        std::dynamic_pointer_cast<Resources::Endpoint::ParentResource>(parent);
    resource = std::make_shared<Resources::Endpoint::CaseResource>(
        case_node->name, case_node->module->name, e_parent);
  } else {
    resource = std::make_shared<Resources::Body::CaseResource>(
        case_node->name, case_node->module->name, parent);
  }
  auto child = case_node->child;
  while (child != nullptr) {
    ParseNode(child, resource, generate_endpoint);
    child = child->next;
  }
  parent->AddChild(std::move(resource));
}

void ParseAny(const lys_node *data,
              const std::shared_ptr<Resources::Body::ParentResource> &parent,
              bool generate_endpoint) {
  bool configuration = ((data->flags & LYS_CONFIG_MASK) ^ 2) != 0;
  bool mandatory = (data->flags & LYS_MAND_MASK) != 0;

  std::unique_ptr<Resources::Body::LeafResource> resource;
  if (generate_endpoint) {
    auto e_parent =
        std::dynamic_pointer_cast<Resources::Endpoint::ParentResource>(parent);
    auto endpoint = e_parent->Endpoint() + data->name;
    resource = std::make_unique<Resources::Endpoint::LeafResource>(
        data->name, data->module->name, endpoint, e_parent,
        std::make_unique<Resources::Body::JsonBodyField>(), configuration,
        mandatory, nullptr);
  } else {
    resource = std::make_unique<Resources::Body::LeafResource>(
        data->name, data->module->name, parent,
        std::make_unique<Resources::Body::JsonBodyField>(), configuration,
        mandatory, nullptr);
  }
  parent->AddChild(std::move(resource));
}

void ParseRpcAction(
    const lys_node_rpc_action *data,
    const std::shared_ptr<Resources::Body::ParentResource> &parent) {
  auto e_parent =
      std::dynamic_pointer_cast<Resources::Endpoint::ParentResource>(parent);
  auto action = std::make_shared<Resources::Endpoint::ParentResource>(data->name, data->module->name, e_parent->Endpoint() + data->name, e_parent);
  auto child = data->child;
  while (child != nullptr) {
    ParseNode(child, action, false);
    child = child->next;
  }
}
// END YANG NODES PARSING
}  // namespace

std::string GetName(const std::string &yang) {
  const auto &context =
      ly_ctx_new("/home/nico/dev/iovnet/services/resources/", 0);
  if (!context) {
    throw std::runtime_error("cannot create new context");
  }

  const auto &module = lys_parse_mem(context, yang.data(), LYS_IN_YANG);
  if (!module) {
    throw std::invalid_argument("invalid yang data");
  }
  return std::string{module->name};
}

std::shared_ptr<Resources::Endpoint::Service> Parse(std::string &&yang) {
  const auto &context =
      ly_ctx_new("/home/nico/dev/iovnet/services/resources/", 0);
  if (!context) {
    throw std::runtime_error("cannot create new context");
  }

  const auto &module = lys_parse_mem(context, yang.data(), LYS_IN_YANG);

  if (!module) {
    throw std::invalid_argument("invalid yang data");
  }

  auto cube = std::make_shared<Resources::Endpoint::Service>(module->name,
                                                             std::string{'/'});
  for (auto i = 0; i < module->imp_size; ++i) {
    ParseModule(module->imp[i].module, cube);
  }
  ParseModule(module, cube);
  typedef_validators_->clear();
  return cube;
}
}  // namespace polycube::polycubed::Rest::Parser
