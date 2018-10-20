//
// Created by nico on 20/10/18.
//

#pragma once

#include <string>
#include <memory>

#include PARSER_INCLUDE
#include "../Resources/Body/Resource.h"
#include "XPathScanner.h"

namespace polycube::polycubed::Rest::Parser {
class XPathParserDriver
{
public:
  XPathParserDriver(std::shared_ptr<Resources::Body::Resource> caller);

  /// enable debug output in the flex scanner
  bool trace_scanning;

  /// enable debug output in the bison parser
  bool trace_parsing;

  /** Invoke the scanner and parser for a stream.
   * @param in	input stream
   * @param sname	stream name for error messages
   * @return		true if successfully parsed
   */
  bool parse_stream(std::istream& in,
                    const std::string& sname = "stream input");

  /** Invoke the scanner and parser on an input string.
   * @param input	input string
   * @param sname	stream name for error messages
   * @return		true if successfully parsed
   */
  bool parse_string(const std::string& input,
                    const std::string& sname = "string stream");

  /** General error handling. This can be modified to output the error
   * e.g. to a dialog box. */
  void error(const std::string& m);

  /** Pointer to the current lexer instance, this is used to connect the
   * parser to the scanner. It is used in the yylex macro. */
  std::unique_ptr<XPathScanner> lexer;

  /** Reference to the calculator context filled during parsing of the
   * expressions. */
  std::shared_ptr<Resources::Body::Resource> current;
};
}
