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
#include "../../include/Parser/XPathParserDriver.h"

namespace polycube::polycubed::Rest::Parser {
XPathParserDriver::XPathParserDriver(
    std::shared_ptr<Resources::Body::Resource> caller)
    : trace_scanning(false), trace_parsing(false), current(std::move(caller)) {}

bool XPathParserDriver::parse_stream(std::istream &in) {
  lexer = std::make_unique<XPathScanner>(&in);
  lexer->set_debug(trace_scanning);

  XPathParser parser{shared_from_this()};
  return parser.parse() == 0;
}

bool XPathParserDriver::parse_string(const std::string &input) {
  std::istringstream iss{input};
  return parse_stream(iss);
}

void XPathParserDriver::error(const std::string &m) {
  std::cerr << m << std::endl;
}
}  // namespace polycube::polycubed::Rest::Parser