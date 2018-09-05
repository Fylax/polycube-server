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

#include "../../include/Resources/LeafResource.h"
#include <pistache/router.h>
#include <string>
#include <memory>
#include "../../include/Error.h"
#include "../../externals/include/nlohmann/json.hpp"

LeafResource::LeafResource(const std::string& name,
                           const std::shared_ptr<Pistache::Rest::Router>& router,
                           const std::string& restEndpoint,
                           const std::shared_ptr<ParentResource>& parent,
                           const std::shared_ptr<JsonBodyField>& field,
                           bool configurable, bool mandatory)
    : Resource(name, router, restEndpoint, parent),
    field_(field), configurable_(configurable), mandatory_(mandatory) {}

Response LeafResource::Validate(const Pistache::Rest::Request& value) const {
  using nlohmann::detail::value_t;
  nlohmann::json body = nlohmann::json::parse(value.body());

  if (body.empty()) return {ErrorTag::kMissingAttribute, name_.c_str()};

  switch (body.type()) {
    case value_t::null:
    case value_t::object:
    case value_t::discarded:
      return {ErrorTag::kBadAttribute, name_.c_str()};
    case value_t::array: {
      bool isInvalid = field_->Type() != JsonType::kEmpty &&
          field_->Type() != JsonType::kList;
      if (isInvalid) return {ErrorTag::kBadAttribute, name_.c_str()};
    }
      break;
    case value_t::string: {
      auto fieldType = field_->Type();
      bool isInvalid = fieldType != JsonType::kString &&
                       fieldType != JsonType::kInt &&
                       fieldType != JsonType::kUint &&
                       fieldType != JsonType::kDecimal;
      if (isInvalid) return {ErrorTag::kBadAttribute, name_.c_str()};
    }
      break;
    case value_t::boolean: {
      bool isInvalid = field_->Type() != JsonType::kBoolean;
      if (isInvalid) return {ErrorTag::kBadAttribute, name_.c_str()};
    }
      break;
    case value_t::number_integer: {
      bool isInvalid = field_->Type() != JsonType::kInt;
      if (isInvalid) return {ErrorTag::kBadAttribute, name_.c_str()};
    }
      break;
    case value_t::number_unsigned: {
      bool isInvalid = field_->Type() != JsonType::kUint;
      if (isInvalid) return {ErrorTag::kBadAttribute, name_.c_str()};
    }
      break;
    case value_t::number_float: {
      bool isInvalid = field_->Type() != JsonType::kDecimal;
      if (isInvalid) return {ErrorTag::kBadAttribute, name_.c_str()};
    }
      break;
  }

  return (field_->Validate(body)) ?
         Response{ErrorTag::kOk, ""} :
         Response{ErrorTag::kBadAttribute, name_.c_str()};
}
