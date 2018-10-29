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
#include "../../../include/Resources/Body/JsonBodyField.h"

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <vector>

#include "../../../include/Types/Decimal64.h"
#include "../../../include/Types/Dummies.h"

namespace polycube::polycubed::Rest::Resources::Body {
JsonBodyField::JsonBodyField()
    : JsonBodyField(LY_TYPE_STRING,
                    std::vector<std::shared_ptr<Validators::Validator>>()) {}

JsonBodyField::JsonBodyField(
    LY_DATA_TYPE type,
    std::vector<std::shared_ptr<Validators::Validator>> &&validators)
    : Field<nlohmann::json>(std::move(validators)),
      allowed_types_(JsonBodyField::FromYangType(type)) {}

ErrorTag JsonBodyField::Validate(const nlohmann::json &value) const {
  if (allowed_types_.count(value.type()) == 0)
    return ErrorTag::kBadAttribute;
  auto parsed = value.get<std::string>();

  for (const auto &validator : validators_) {
    if (!validator->Validate(parsed))
      return ErrorTag::kBadAttribute;
  }
  return ErrorTag::kOk;
}

const std::unordered_set<std::type_index> JsonBodyField::AcceptableTypes(
    nlohmann::detail::value_t type) {
  std::unordered_set<std::type_index> types;
  switch (type) {
  case nlohmann::detail::value_t::null:
    break;
  case nlohmann::detail::value_t::object:
    break;
  case nlohmann::detail::value_t::array:
    types.reserve(2);
    types.insert(std::type_index(typeid(Types::Empty)));
    types.insert(std::type_index(typeid(Types::List)));
    break;
  case nlohmann::detail::value_t::string:
    types.reserve(7);
    types.insert(std::type_index(typeid(std::string)));
    types.insert(std::type_index(typeid(std::int64_t)));
    types.insert(std::type_index(typeid(std::uint64_t)));
    types.insert(std::type_index(typeid(Types::Bits)));
    types.insert(std::type_index(typeid(Types::Enum)));
    types.insert(std::type_index(typeid(Types::Decimal64)));
    types.insert(std::type_index(typeid(Types::XPath)));
    break;
  case nlohmann::detail::value_t::boolean:
    types.reserve(1);
    types.insert(std::type_index(typeid(bool)));
    break;
  case nlohmann::detail::value_t::number_integer:
    types.reserve(4);
    types.insert(std::type_index(typeid(std::int8_t)));
    types.insert(std::type_index(typeid(std::int16_t)));
    types.insert(std::type_index(typeid(std::int32_t)));
    types.insert(std::type_index(typeid(std::int64_t)));
    break;
  case nlohmann::detail::value_t::number_unsigned:
    types.reserve(4);
    types.insert(std::type_index(typeid(std::uint8_t)));
    types.insert(std::type_index(typeid(std::uint16_t)));
    types.insert(std::type_index(typeid(std::uint8_t)));
    types.insert(std::type_index(typeid(std::uint64_t)));
    break;
  case nlohmann::detail::value_t::number_float:
    types.reserve(1);
    types.insert(std::type_index(typeid(Types::Decimal64)));
    break;
  case nlohmann::detail::value_t::discarded:
    break;
  }
  return types;
}

const std::unordered_set<nlohmann::detail::value_t> JsonBodyField::FromYangType(
    LY_DATA_TYPE type) {
  using nlohmann::detail::value_t;
  std::unordered_set<nlohmann::detail::value_t> allowed;
  switch (type) {
  case LY_TYPE_BOOL:
    allowed.insert(value_t::boolean);
    return allowed;
  case LY_TYPE_EMPTY:
    allowed.insert(value_t::array);
    return allowed;
  case LY_TYPE_BINARY:
  case LY_TYPE_BITS:
  case LY_TYPE_ENUM:
  case LY_TYPE_INST:
  case LY_TYPE_LEAFREF:
  case LY_TYPE_STRING:
    allowed.insert(value_t::string);
    return allowed;
  case LY_TYPE_INT64:
    allowed.insert(value_t::string);
    allowed.insert(value_t::number_integer);
    return allowed;
  case LY_TYPE_UINT64:
    allowed.insert(value_t::string);
    allowed.insert(value_t::number_unsigned);
    return allowed;
  case LY_TYPE_DEC64:
    allowed.insert(value_t::string);
    allowed.insert(value_t::number_float);
    return allowed;
  case LY_TYPE_INT8:
  case LY_TYPE_INT16:
  case LY_TYPE_INT32:
    allowed.insert(value_t::number_integer);
    return allowed;
  case LY_TYPE_UINT8:
  case LY_TYPE_UINT16:
  case LY_TYPE_UINT32:
    allowed.insert(value_t::string);
    allowed.insert(value_t::number_unsigned);
    return allowed;
  case LY_TYPE_UNION:
    allowed.insert(value_t::array);
    allowed.insert(value_t::string);
    allowed.insert(value_t::boolean);
    allowed.insert(value_t::number_integer);
    allowed.insert(value_t::number_unsigned);
    allowed.insert(value_t::number_float);
    return allowed;
  default:
    throw std::runtime_error("Unsupported type");
  }
}
}  // namespace polycube::polycubed::Rest::Resources::Body
