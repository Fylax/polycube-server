#pragma once

// $Id$

#ifndef EXAMPLE_SCANNER_H
#define EXAMPLE_SCANNER_H

// Flex expects the signature of yylex to be defined in the macro YY_DECL, and
// the C++ parser expects it to be declared. We can factor both as follows.

#ifndef YY_DECL

#define	YY_DECL						\
    polycube::polycubed::Rest::Parser::XPathParser::token_type				\
    polycube::polycubed::Rest::Parser::XPathScanner::lex(				\
	polycube::polycubed::Rest::Parser::XPathParser::semantic_type* yylval,		\
	polycube::polycubed::Rest::Parser::XPathParser::location_type* yylloc		\
    )
#endif

#ifndef __FLEX_LEXER_H
#include "FlexLexer.h"
#endif

#include PARSER_INCLUDE

namespace polycube::polycubed::Rest::Parser {

/** Scanner is a derived class to add some extra function to the scanner
 * class. Flex itself creates a class named yyFlexLexer, which is renamed using
 * macros to ExampleFlexLexer. However we change the context of the generated
 * yylex() function to be contained within the Scanner class. This is required
 * because the yylex() defined in ExampleFlexLexer has no parameters. */
class XPathScanner : public yyFlexLexer
{
public:
  /** Create a new scanner object. The streams arg_yyin and arg_yyout default
   * to cin and cout, but that assignment is only made when initializing in
   * yylex(). */
  XPathScanner(std::istream* arg_yyin = 0);

  /** Required for virtual functions */
  ~XPathScanner() override = default;

  /** This is the main lexing function. It is generated by flex according to
   * the macro declaration YY_DECL above. The generated bison parser then
   * calls this virtual function to fetch new tokens. */
  virtual XPathParser::token_type lex(
      XPathParser::semantic_type* yylval,
      XPathParser::location_type* yylloc
  );

  /** Enable debug output (via arg_yyout) if compiled into the scanner. */
  void set_debug(bool b);
};

} // namespace example

#endif // EXAMPLE_SCANNER_H
