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
#include "../../../include/Resources/Body/ListResource.h"

#include <ListResource.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../../include/Resources/Body/ListKey.h"
#include "../../../include/Validators/Validator.h"

namespace polycube::polycubed::Rest::Resources::Body {
ListResource::ListResource(std::vector<ListKey> &&keys)
    : ParentResource("", "", nullptr, false), keys_{std::move(keys)} {}

ListResource::ListResource(std::string name, std::string module,
                           std::shared_ptr<ParentResource> parent,
                           std::vector<ListKey> &&keys)
    : ParentResource(std::move(name), std::move(module), std::move(parent),
                     false),
      keys_{std::move(keys)} {}

std::vector<Response> ListResource::MultipleBodyValidate(nlohmann::json &body,
                                                         bool update) const {
  std::vector<Response> errors;
  if (body.type() != nlohmann::detail::value_t::array) {
    errors.push_back({ErrorTag::kBadAttribute, name_.data()});
    return errors;
  }

  // TODO get list instantiated values (key needed)
  for (auto element : body) {
    bool check_mandatory = !update;  // TODO or exists name in list
    auto element_errors = BodyValidate(element, check_mandatory);
    errors.reserve(errors.size() + element_errors.size());
    std::copy(std::begin(element_errors), std::end(element_errors),
              std::back_inserter(errors));
  }
  return errors;
}

bool ListResource::ValidateXPath(const std::string &xpath) const {
  auto del_pos = xpath.find('/');  // current delimiter
  auto ns_pos = xpath.find(':');   // current namespace delimiter

  std::string name;
  if (ns_pos < del_pos) {  // fully-qualified name
    name = xpath.substr(ns_pos + 1, del_pos);
  } else {
    name = xpath.substr(0, del_pos);
  }
  if (name != name_)
    return false;

  // Find first key delimiter. It is required both in
  // case of list with key that in list without keys.
  auto key_pos = xpath.find('[');  // first key position
  auto end_key_pos = xpath.find(']', key_pos);

  // Validation changes depending on whether keys were defined.
  if (!keys_.empty()) {
    for (const auto &key : keys_) {
      // key is mandatory (and well-formed)
      if (key_pos > del_pos || end_key_pos > del_pos)
        return false;

      // A key has two possible formats:
      // [name='content'] or [name]
      auto eq_pos = xpath.find('=', key_pos + 1);
      auto end_key_name = std::min(eq_pos, end_key_pos);
      // valid for both first and second case
      auto key_name = xpath.substr(key_pos + 1, end_key_name);
      if (key_name != key.Name())
        return false;

      // first case
      if (eq_pos < end_key_pos) {
        auto start_content_pos = eq_pos + 1;
        if (xpath[start_content_pos] != '\'')
          return false;
        auto end_content_pos = xpath.find('\'', start_content_pos + 1);
        auto content = xpath.substr(start_content_pos + 1, end_content_pos);
        for (const auto &validator : key.Validators())
          if (!validator->Validate(content))
            return false;
      }
      // get next key delimiters
      key_pos =
          (xpath[end_key_pos + 1] == '[') ? end_key_pos + 1 : std::string::npos;
      end_key_pos = xpath.find(']', key_pos);
    }
  } else {
    // key is not mandatory
    if (key_pos > del_pos || end_key_pos > del_pos)
      return false;
  }

  return ValidateXPathChildren(xpath, del_pos);
}

bool ListResource::ValidateKeys(
    std::unordered_map<std::string, std::string> keys) const {
  for (const auto &key : keys_) {
    if (keys.count(key.Name()) == 0)
      return false;
    const auto &value = keys.at(key.Name());
    for (const auto &validator : key.Validators()) {
      if (!validator->Validate(value))
        return false;
    }
  }
  return true;
}

const Response ListResource::ReadValue(
    const std::string& cube_name, PerListKeyValues& keys) const {
  const auto &parent_value = Resource::ReadValue(cube_name, keys);
  auto jparent = nlohmann::json::parse(parent_value.message);
  for (auto element : jparent) {
    unsigned matching_keys = 0;
    const auto &current_keys = keys.top();
    keys.pop();
    for (const auto &key : current_keys) {
      if (element[key.first.Name()] == key.second)
        matching_keys += 1;
    }
    if (matching_keys == current_keys.size())
      return {ErrorTag::kOk, element.dump().data()};
  }
  throw std::runtime_error(
      "Unreachable since keys are mandatory and validated.");
}

const Response ListResource::ReadWhole(const std::string& cube_name,
                                       PerListKeyValues& keys) const {
  return ParentResource::ReadValue(cube_name, keys);
}
}  // namespace polycube::polycubed::Rest::Resources::Body
