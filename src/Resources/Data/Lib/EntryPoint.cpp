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
#include "../../../../include/Resources/Data/Lib/EntryPoint.h"

#include <dlfcn.h>

#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <string_view>

#include "../../../../include/Resources/Body/ParentResource.h"
#include "../../../../include/Resources/Body/Resource.h"

namespace polycube::polycubed::Rest::Resources::Data::Lib::EntryPoint {
namespace {
using namespace std::string_view_literals;
struct OperationName {
  Operation op;
  const std::string_view name;
};
constexpr OperationName operation_names_[] = {
    {Operation::kCreate, "create_"sv},
    {Operation::kRead, "read_"sv},
    {Operation::kReplace, "replace_"sv},
    {Operation::kUpdate, "update_"sv},
    {Operation::kDelete, "delete_"sv}};
}  // namespace

std::string GenerateName(std::queue<std::string> names, Operation operation) {
  std::string_view operation_name =
      std::find_if(std::begin(operation_names_), std::end(operation_names_),
                   [=](const OperationName &on) { return on.op == operation; })
          ->name;

  std::string entry_point_name{operation_name};
  while (!names.empty()) {
    entry_point_name.append(names.front() + '_');
    names.pop();
  }

  // normalize name (replace all - and . with _, since cannot be
  // part of a function name)
  std::replace(std::begin(entry_point_name) + 5, std::end(entry_point_name),
               '-', '_');
  std::replace(std::begin(entry_point_name) + 5, std::end(entry_point_name),
               '.', '_');
  entry_point_name.append("by_id");
  return entry_point_name;
}
}  // namespace polycube::polycubed::Rest::Resources::Data::Lib::EntryPoint
