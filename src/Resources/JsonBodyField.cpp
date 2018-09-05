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
#include "../../include/Resources/JsonBodyField.h"

#include <string>
#include <memory>
#include <vector>
#include "../../include/Validators/Validator.h"

JsonBodyField::JsonBodyField(
    const std::vector<std::shared_ptr<Validator>>& validators, JsonType type)
    : Field<nlohmann::json>(validators), type_(type) {}

ErrorTag JsonBodyField::Validate(const nlohmann::json& value) const {
  auto parsed = value.get<std::string>();
  for (const auto& validator : validators_) {
    if (!validator->Validate(parsed)) return ErrorTag::kBadAttribute;
  }
  return ErrorTag::kOk;
}

JsonType JsonBodyField::Type() const {
  return type_;
}

JsonType JsonBodyField::FromYangType(LY_DATA_TYPE type) {
  switch (type) {
    case LY_TYPE_BOOL:
      return JsonType::kBoolean;
    case LY_TYPE_DEC64:
      return JsonType::kDecimal;
    case LY_TYPE_EMPTY:
      return JsonType::kEmpty;
    case LY_TYPE_BINARY:
    case LY_TYPE_BITS:
    case LY_TYPE_ENUM:
    case LY_TYPE_LEAFREF:
    case LY_TYPE_STRING:
    case LY_TYPE_UNION:
    case LY_TYPE_INT64:
    case LY_TYPE_UINT64:
      return JsonType::kString;
    case LY_TYPE_INT8:
    case LY_TYPE_INT16:
    case LY_TYPE_INT32:
      return JsonType::kInt;
    case LY_TYPE_UINT8:
    case LY_TYPE_UINT16:
    case LY_TYPE_UINT32:
      return JsonType::kUint;
    default:
      throw std::runtime_error("unsupported type");
  }
}
