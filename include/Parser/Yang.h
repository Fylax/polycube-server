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
#include <queue>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "../Resources/Data/AbstractFactory.h"

namespace polycube::polycubed::Rest::Validators {
class Validator;
class LengthValidator;
}  // namespace polycube::polycubed::Rest::Validators

namespace polycube::polycubed::Rest::Resources::Body {
class ParentResource;
}

namespace polycube::polycubed::Rest::Resources::Endpoint {
class Service;
}

namespace polycube::polycubed::Rest::Parser {
using ValidatorList = std::vector<std::shared_ptr<Validators::Validator>>;
using ValidatorsType =
    std::pair<const ValidatorList, const std::unordered_set<std::type_index>>;

class Yang {
 public:
  explicit Yang(std::unique_ptr<Resources::Data::AbstractFactory> &&factory);
  static const std::string ServiceName(const std::string &yang);
  const std::shared_ptr<Resources::Endpoint::Service> Parse();

 private:
  std::unique_ptr<Resources::Data::AbstractFactory> factory_;
  /** Stores typedef name and associated list of Validators */
  std::unordered_map<std::string, ValidatorList>
      typedef_validators_[LY_DATA_TYPE_COUNT];

  const ValidatorList GetValidators(lys_type type) const;

  static const ValidatorsType ParseType(lys_type type);

  template <typename T>
  static const std::unordered_map<T, T> ParseRange(std::string_view &range);

  static const std::shared_ptr<Validators::LengthValidator> ParseLength(
      const lys_restr *length, bool binary);

  static const ValidatorsType ParseEnum(lys_type_info_enums enums);

  static const ValidatorsType ParseString(lys_type_info_str str);

  template <typename T>
  static const ValidatorsType ParseInteger(lys_type_info_num num);

  static const ValidatorsType ParseDecimal64(lys_type_info_dec64 dec64);

  static const ValidatorsType ParseBits(lys_type_info_bits bits);

  static const ValidatorsType ParseBinary(lys_type_info_binary binary);

  static const ValidatorsType ParseLeafRef(lys_type_info_lref lref);

  static const ValidatorsType ParseBoolean();

  static const ValidatorsType ParseEmpty();

  static const ValidatorsType ParseUnion(lys_type_info_union yunion);

  static const ValidatorsType ParseInstanceIdentifier(lys_type_info_inst iid,
                                                      const char *context);

  void ParseModule(const lys_module *module, std::queue<std::string> parsed_names,
                   const std::shared_ptr<Resources::Endpoint::Service> &cube);

  void ParseNode(const lys_node *data, std::queue<std::string> parsed_names,
                 const std::shared_ptr<Resources::Body::ParentResource> &parent,
                 bool generate_endpoint) const;

  void ParseContainer(
      const lys_node_container *data, std::queue<std::string> parsed_names,
      const std::shared_ptr<Resources::Body::ParentResource> &parent,
      bool generate_endpoint) const;

  void ParseUses(
      const lys_node_uses *group, std::queue<std::string> parsed_names,
      const std::shared_ptr<Resources::Body::ParentResource> &parent,
      bool generate_endpoint) const;

  void ParseList(const lys_node_list *list,
                 std::queue<std::string> parsed_names,
                 const std::shared_ptr<Resources::Body::ParentResource> &parent,
                 bool generate_endpoint) const;

  void ParseLeaf(const lys_node_leaf *leaf,
                 std::queue<std::string> parsed_names,
                 const std::shared_ptr<Resources::Body::ParentResource> &parent,
                 bool generate_endpoint) const;

  void ParseLeafList(
      const lys_node_leaflist *leaflist, std::queue<std::string> parsed_names,
      const std::shared_ptr<Resources::Body::ParentResource> &parent,
      bool generate_endpoint) const;

  void ParseChoice(
      const lys_node_choice *choice, std::queue<std::string> parsed_names,
      const std::shared_ptr<Resources::Body::ParentResource> &parent,
      bool generate_endpoint) const;

  void ParseCase(const lys_node_case *case_node,
                 std::queue<std::string> parsed_names,
                 const std::shared_ptr<Resources::Body::ParentResource> &parent,
                 bool generate_endpoint) const;

  void ParseAny(const lys_node *data, std::queue<std::string> parsed_names,
                const std::shared_ptr<Resources::Body::ParentResource> &parent,
                bool generate_endpoint) const;

  void ParseRpcAction(
      const lys_node_rpc_action *data, std::queue<std::string> parsed_names,
      const std::shared_ptr<Resources::Body::ParentResource> &parent) const;
};

}  // namespace polycube::polycubed::Rest::Parser
