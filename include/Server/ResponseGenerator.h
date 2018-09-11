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
#ifndef PARSER_RESPONSEGENERATOR_H
#define PARSER_RESPONSEGENERATOR_H

#include <pistache/http.h>
#include "../../externals/include/nlohmann/json.hpp"
#include "Error.h"

namespace ResponseGenerator {
using nlohmann::json;
namespace {
json error = {
    {
        "error", {
                     {"error-type", "application"},
                     {"error-tag", ""}
                 }
    }
};
json errors = {
    {
        "ietf-restconf:errors", {}
    }
};
}

void Generate(std::vector<Response>&& response,
              Pistache::Http::ResponseWriter&& writer);
}  // namespace ResponseGenerator

#endif //PARSER_RESPONSEGENERATOR_H
