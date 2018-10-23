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
#include "../../../include/Resources/Body/ListKey.h"

#include <ListKey.h>
#include <memory>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

namespace polycube::polycubed::Rest::Resources::Body {
ListKey::ListKey(
    LY_DATA_TYPE type, std::string name,
    std::vector<
        std::shared_ptr<polycube::polycubed::Rest::Validators::Validator>>
        &&validators)
    : type_(FromYang(type)),
      name_(std::move(name)),
      validators_(std::move(validators)) {}

const std::type_index ListKey::Type() const {
  return type_;
}

const std::string ListKey::Name() const {
  return name_;
}

const std::vector<std::shared_ptr<Validators::Validator>> &ListKey::Validators()
    const {
  return validators_;
}

std::type_index ListKey::FromYang(LY_DATA_TYPE type) {
  switch (type) {
  case LY_TYPE_BOOL:
    return std::type_index(typeid(bool));
  case LY_TYPE_BINARY:
  case LY_TYPE_BITS:
  case LY_TYPE_ENUM:
  case LY_TYPE_IDENT:
  case LY_TYPE_INST:
  case LY_TYPE_STRING:
  case LY_TYPE_UNION:
  case LY_TYPE_DEC64:
    return std::type_index(typeid(std::string));
  case LY_TYPE_EMPTY:
    return std::type_index(typeid(std::string));
  case LY_TYPE_INT8:
    return std::type_index(typeid(std::int8_t));
  case LY_TYPE_UINT8:
    return std::type_index(typeid(std::uint8_t));
  case LY_TYPE_INT16:
    return std::type_index(typeid(std::int16_t));
  case LY_TYPE_UINT16:
    return std::type_index(typeid(std::uint16_t));
  case LY_TYPE_INT32:
    return std::type_index(typeid(std::int32_t));
  case LY_TYPE_UINT32:
    return std::type_index(typeid(std::uint32_t));
  case LY_TYPE_INT64:
    return std::type_index(typeid(std::int64_t));
  case LY_TYPE_UINT64:
    return std::type_index(typeid(std::uint64_t));
    default:
      throw std::runtime_error("Unrecognized YANG data type");
  }
}
}  // namespace polycube::polycubed::Rest::Resources::Body

