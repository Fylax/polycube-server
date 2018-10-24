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
#include "../../../../../include/Resources/Data/Lib/Body/EntryPoint.h"

#include <memory>
#include <stack>
#include <string>
#include <string_view>

#include "../../../../../include/Resources/Body/ParentResource.h"
#include "../../../../../include/Resources/Body/Resource.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib::EntryPoint {
namespace {
using namespace std::string_view_literals;
struct OperationName {
  Operation op;
  const std::string_view name;
};
constexpr OperationName operation_names_[] = {
    {Operation::kCreate, "create_"sv},
    {Operation::kReplace, "replace_"sv},
    {Operation::kUpdate, "update_"sv},
    {Operation::kDelete, "delete_"sv}};
};  // namespace

std::string GenerateName(std::shared_ptr<Resources::Body::Resource> resource,
                         Operation operation) {
  std::stack<std::string> parent_names;
  parent_names.emplace("by_id");
  parent_names.emplace(resource->Name() + '_');
  std::size_t name_length = 6 + resource->Name().length();

  std::shared_ptr<Resources::Body::ParentResource> current = resource->Parent();
  while (current != nullptr) {
    parent_names.emplace(current->Name() + '-');
    name_length += current->Name().length() + 1;
    current = current->Parent();
  }

  std::string_view operation_name =
      std::find_if(std::begin(operation_names_), std::end(operation_names_),
                   [=](const OperationName &on) { return on.op == operation; })
          ->name;

  std::string entry_point_name{operation_name};
  entry_point_name.reserve(name_length);
  while (!parent_names.empty()) {
    entry_point_name.append(parent_names.top());
    parent_names.pop();
  }
  return entry_point_name;
}
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib::EntryPoint