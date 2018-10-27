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
#pragma once

#include <functional>
#include <memory>
#include <string>

namespace polycube::polycubed::Rest::Resources::Body {
class Resource;
}

namespace polycube::polycubed::Rest::Resources::Data::Lib::EntryPoint {
enum class Operation { kCreate, kRead, kUpdate, kReplace, kDelete };

std::string GenerateName(std::shared_ptr<Resources::Body::Resource> resource,
                         Operation operation);

}  // namespace polycube::polycubed::Rest::Resources::Data::Lib::EntryPoint

#ifdef __cplusplus
extern "C" {
#endif
enum ElementType { INTEGER, UNSIGNED_INTEGER, BOOL, DECIMAL, STRING, EMPTY };

typedef struct {
  ElementType type;
  union {
    long long integer;
    unsigned long long unsigned_integer;
    bool boolean;
    const char *string;
  };
} Element;

typedef struct {
  Element element;
  const char *name;
} Key;
#ifdef __cplusplus
}
#endif
