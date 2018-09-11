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
#include <utility>
#include "../../include/Server/Error.h"
#include "../../include/Resources/ParentResource.h"
#include "../../externals/include/nlohmann/json.hpp"

using Pistache::Rest::Routes::bind;
using Pistache::Rest::Request;
using Pistache::Http::ResponseWriter;

LeafResource::LeafResource(const std::string& name,
                           const std::shared_ptr<Pistache::Rest::Router>& router,
                           const std::string& restEndpoint,
                           const std::shared_ptr<ParentResource>& parent,
                           std::unique_ptr<JsonBodyField>&& field,
                           bool configurable, bool mandatory,
                           std::unique_ptr<const std::string>&& default_value)
    : Resource(name, router, restEndpoint, parent),
    field_(std::move(field)), configurable_(configurable), mandatory_(mandatory),
    default_(std::move(default_value)) {
  router_->get(restEndpoint, bind(&LeafResource::get, this));
}

std::vector<Response> LeafResource::Validate(const Pistache::Rest::Request& value) const {
  using nlohmann::detail::value_t;
  nlohmann::json body = nlohmann::json::parse(value.body());

  auto errors = parent_->Validate(value);
  if (errors[0].error_tag == kOk) {
    errors.pop_back();
  }

  if (body.empty()) {
    errors.push_back({ErrorTag::kMissingAttribute, name_.c_str()});
    return errors;
  }

  switch (body.type()) {
    case value_t::null:
    case value_t::object:
    case value_t::discarded:
      errors.push_back({ErrorTag::kBadAttribute, name_.c_str()});
    case value_t::array: {
      bool isInvalid = field_->Type() != JsonType::kEmpty &&
          field_->Type() != JsonType::kList;
      if (isInvalid) errors.push_back({ErrorTag::kBadAttribute, name_.c_str()});
    }
      break;
    case value_t::string: {
      auto fieldType = field_->Type();
      bool isInvalid = fieldType != JsonType::kString &&
                       fieldType != JsonType::kInt &&
                       fieldType != JsonType::kUint &&
                       fieldType != JsonType::kDecimal;
      if (isInvalid) errors.push_back({ErrorTag::kBadAttribute, name_.c_str()});
    }
      break;
    case value_t::boolean: {
      bool isInvalid = field_->Type() != JsonType::kBoolean;
      if (isInvalid) errors.push_back({ErrorTag::kBadAttribute, name_.c_str()});
    }
      break;
    case value_t::number_integer: {
      bool isInvalid = field_->Type() != JsonType::kInt;
      if (isInvalid) errors.push_back({ErrorTag::kBadAttribute, name_.c_str()});
    }
      break;
    case value_t::number_unsigned: {
      bool isInvalid = field_->Type() != JsonType::kUint;
      if (isInvalid) errors.push_back({ErrorTag::kBadAttribute, name_.c_str()});
    }
      break;
    case value_t::number_float: {
      bool isInvalid = field_->Type() != JsonType::kDecimal;
      if (isInvalid) errors.push_back({ErrorTag::kBadAttribute, name_.c_str()});
    }
      break;
  }

  auto field = field_->Validate(body);
  if (field == kOk) {
    if (errors.empty()) {
      errors.push_back({ErrorTag::kOk, ""});
    }
  } else {
    errors.push_back({field, name_.c_str()});
  }
  return errors;
}

bool LeafResource::IsMandatory() const {
  return mandatory_;
}

bool LeafResource::HasDefault() const {
  return default_ != nullptr;
}

void LeafResource::get(const Request& request, ResponseWriter response) {
  auto resp = Resource::parent_->Validate(request);

}
