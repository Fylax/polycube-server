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
#include "../../include/Validators/UnionValidator.h"
#include <cstdint>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <utility>
#include <vector>
#include "../../include/Types/Decimal64.h"
#include "../../include/Types/Dummies.h"

UnionValidator::UnionValidator(): map_{} {}

void UnionValidator::AddType(std::type_index type,
                             const std::vector<
                                 std::shared_ptr<Validator>
                             >& validators) {
  map_.emplace(type, validators);
}

bool UnionValidator::Validate(const std::string& value) const {
  auto data = nlohmann::json::parse(value);
  const auto& allowed = UnionValidator::AcceptableTypes(data.type());
  for (const auto& union_type : map_) {
    if (allowed.count(union_type.first) != 0) {
      for (const auto& validator : union_type.second) {
        if (validator->Validate(value)) return true;
      }
    }
  }
  return false;
}

const std::unordered_set<std::type_index>
UnionValidator::AcceptableTypes(nlohmann::detail::value_t type) {
  std::unordered_set<std::type_index> types;
  switch (type) {
    case nlohmann::detail::value_t::null:
      break;
    case nlohmann::detail::value_t::object:
      break;
    case nlohmann::detail::value_t::array:
      types.reserve(2);
      types.insert(std::type_index(typeid(Empty)))
      // TODO not managed (yet)
      // TODO ListType
      break;
    case nlohmann::detail::value_t::string:
      types.reserve(6);
      types.insert(std::type_index(typeid(std::string)));
      types.insert(std::type_index(typeid(std::int64_t)));
      types.insert(std::type_index(typeid(std::uint64_t)));
      types.insert(std::type_index(typeid(Bits)));
      types.insert(std::type_index(typeid(Enum)));
      types.insert(std::type_index(typeid(Decimal64)));
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
      types.insert(std::type_index(typeid(Decimal64)));
      break;
    case nlohmann::detail::value_t::discarded:
      break;
  }
  return types;
}
