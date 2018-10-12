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

namespace polycube::polycubed::Rest::Resources::Body {
ListResource::ListResource(
    std::vector<std::pair<std::string,
                          std::vector<std::shared_ptr<Validators::Validator>>>>
        &&keys)
    : ParentResource("", "", nullptr, false), keys_{std::move(keys)} {}

ListResource::ListResource(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent,
    std::vector<std::pair<std::string,
                          std::vector<std::shared_ptr<Validators::Validator>>>>
        &&keys)
    : ParentResource(std::move(name), std::move(module), std::move(parent),
                     false),
      keys_{std::move(keys)} {}

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
      if (key_name != key.first)
        return false;

      // first case
      if (eq_pos < end_key_pos) {
        auto start_content_pos = eq_pos + 1;
        if (xpath[start_content_pos] != '\'')
          return false;
        auto end_content_pos = xpath.find('\'', start_content_pos + 1);
        auto content = xpath.substr(start_content_pos + 1, end_content_pos);
        for (const auto &validator : key.second)
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
}  // namespace polycube::polycubed::Rest::Resources::Body
// namespace polycube::polycubed::Rest::Resources::Body
