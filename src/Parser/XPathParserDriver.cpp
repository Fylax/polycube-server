//
// Created by nico on 20/10/18.
//

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