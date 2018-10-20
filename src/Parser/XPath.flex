%{
#include <string>

#include "../../include/Parser/XPathParserDriver.h"
#include "XPathParser.h"

namespace polycube::polycubed::Rest::Parser {
using token = XPathParser::token;
#define yyterminate() return(token::END)
#define YY_NO_UNISTD_H
#define YY_USER_ACTION loc->columns(yyleng); yylloc->step();
%}

%option nounput noinput nomain noyywrap nodefault
%option yyclass="XPathScanner"

NOT     not
TRUE    true\(\)
FALSE   false\(\)
OR      or
AND     and
EQ      =
NEQ     !=
LT      <
LTE     <=
GT      >
GTE     >=

CURRENT current\(\)|.
PARENT  ..
PATH    \/
ID      [a-zA-Z_][a-zA-Z0-9_.-]*
SQSRT   \'[^\']*\'
INTEGER [1-9][0-9]*
WS      [ \t]+

%%

%{
  yylval = lval;
%}

NOT     {return token::NOT;}
TRUE    {return token::TRUE;}
FALSE   {return token::FALSE;}
OR      {return token::OR;}
AND     {return token::AND;}
EQ      {return token::EQ;}
NEQ     {return token::NEQ;}
LT      {return token::LT;}
LTE     {return token::LTE;}
GT      {return token::GT;}
GTE     {return token::GTE;}
CURRENT {return token::CUR;}
PARENT  {return token::PAR;}
PATH    {return token::DEL;}
\(      {return token::RO;}
\)      {return token::RC;}
\[      {return token::SO;}
\]      {return token::SC;}
ID      {
          yylval->build<std::string>(yytext);
          return token::ID;
        }
SQSRT   {
          yylval->build<std::string>(yytext);
          return token::SQSRT;
        }
INTEGER {
          yylval->build<std::uint32_t>(yytext);
          return token::INT;
        }

{WS}|.  {}
%%

XPathScanner::XPathScanner(std::istream* in)
    : yyFlexLexer(in, 0){}

void XPathScanner::set_debug(bool b)
{
    yy_flex_debug = b;
}

#ifdef yylex
#undef yylex
#endif

int XPathScanner::yylex()
{
    std::cerr << "in yyFlexLexer::yylex() !" << std::endl;
    return 0;
}

int yyFlexLexer::yywrap()
{
    return 1;
}

void
XPathParserDriver::scan_begin()
{
  yy_flex_debug = false;
  yy_scan_string(string.data());
}
}
