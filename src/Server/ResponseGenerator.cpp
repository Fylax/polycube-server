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
#include "../../include/Server/ResponseGenerator.h"

#include <vector>

namespace polycube::polycubed::Rest::Server {
namespace {
using nlohmann::json;
auto error = R"({
  "error-type": "application",
  "error-tag": ""
})"_json;

auto errors = R"({
  "ietf-restconf:errors": {
    "error": []
  }
})"_json;
}  // namespace

void ResponseGenerator::Generate(std::vector<Response> &&response,
                                 Pistache::Http::ResponseWriter &&writer) {
  using Pistache::Http::Code;
  auto mime = Pistache::Http::Mime::MediaType(
      "application/yang.data+json; charset=utf-8");
  writer.headers().add<Pistache::Http::Header::ContentType>(mime);
  if (response[0].error_tag == kOk) {
    writer.send(Code::Ok, response[0].message, mime);
    return;
  }
  if (response[0].error_tag == kCreated) {
    writer.send(Code::Created, response[0].message, mime);
    return;
  }
  auto body = errors;
  Code response_code = Code::Ok;
  for (const auto &err : response) {
    auto single = error;
    switch (err.error_tag) {
    case kOk:
    case kCreated:
      break;
    case kInvalidValue:
      response_code = Code::Bad_Request;
      single["error-tag"] = "invalid-value";
      break;
    case kMissingAttribute:
      response_code = Code::Bad_Request;
      single["error-tag"] = "missing-attribute";
      break;
    case kMissingElement:
      response_code = Code::Bad_Request;
      single["error-tag"] = "missing-element";
      break;
    case kBadAttribute:
      response_code = Code::Bad_Request;
      single["error-tag"] = "bad-attribute";
      break;
    case kBadElement:
      response_code = Code::Bad_Request;
      single["error-tag"] = "bad-element";
      break;
    case kDataExists:
      response_code = Code::Conflict;
      single["error-tag"] = "data-exists";
      break;
    case kDataMissing:
      response_code = Code::Conflict;
      single["error-tag"] = "data-missing";
      break;
    case kOperationNotSupported:
      response_code = Code::Method_Not_Allowed;
      single["error-tag"] = "operation-not-supported";
      break;
    }
    if (std::strlen(err.message))
      single["error-info"] = err.message;

    body["ietf-restconf:errors"]["error"].push_back(single);
  }
  writer.send(response_code, body.dump(), mime);
}
}  // namespace polycube::polycubed::Rest::Server
