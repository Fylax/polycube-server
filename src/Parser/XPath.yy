%skeleton "lalr1.cc"
%require "3.0"
%define api.namespace {polycube::polycubed::Rest::Parser}
%define parser_class_name {XPathParser}
%language "C++"
%no-lines

%code requires{
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <vector>
#include <utility>
#include "../../include/Resources/CubeManager.h"
#include "../../include/Resources/Body/Resource.h"
#include "../../include/Resources/Body/ParentResource.h"
#include "../../include/Resources/Body/ListResource.h"

namespace polycube::polycubed::Rest::Parser {
  class XPathParserDriver;
}
}


%define api.value.type variant
%define parse.assert

%param {XPathParserDriver &driver}

%locations

%token END    0     "end of file"

%token NOT
%token TRUE
%token FALSE
%token OR
%token AND
%token EQ
%token NEQ
%token LT
%token LTE
%token GT
%token GTE
%token CUR
%token PAR
%token DEL
%token RO
%token RC
%token SO
%token SC
%token <std::string> ID
%token <std::string> SQSTR
%token <std::uint32_t> INT

%left AND
%left OR
%left EQ NEQ LT LTE GT GTE

%type <bool> program;
%type <bool> expr;
%type <bool> bool_expr;
%type <std::string> start_path;
%type <std::string> path;

%type <std::vector<std::pair<std::string, std::string>>> key;
%type <std::vector<std::pair<std::string, std::string>>> keys;
%type <std::vector<std::pair<std::string, std::string>>> key_list;

%{
#include "../../include/Parser/XPathParserDriver.h"
#include "../../include/Parser/XPathScanner.h"

#undef yylex
#define yylex driver.lexer->lex

%}

%%

program : END { $$ = false; } | expr END { $$ = $1; };

expr : start_path { $$ = !$1.empty() && $1 != "0"; } | bool_expr { $$ = $1; };

bool_expr : expr EQ expr { $$ = $1 == $3; } |
            expr NEQ expr { $$ = $1 != $3; } |
            expr LT expr { $$ = $1 < $3; } |
            expr LTE expr { $$ = $1 < $3; } |
            expr GT expr { $$ = $1 < $3; } |
            expr GTE expr { $$ = $1 < $3; } |
            NOT RO expr RC { $$ = !$3; } |
            expr AND expr { $$ = $1 && $3; } |
            expr OR expr { $$ = $1 || $3; } |
            TRUE { $$ = true; } |
            FALSE { $$ = false; };

start_path : DEL ID DEL ID { driver.current = CubeManager.GetInstance.Cube($2)-Child($4); } path { $$ = $6; } |
             CUR { $$ = resource->Value(); } |
             CUR DEL { driver.current = resource; } path { $$ = $4; } |
             PAR { $$ = resource->Parent()->Value(); } |
             PAR DEL { driver.current = resource->Parent(); } path { $$ = $4; } |
             ID { $$ = driver.resource->Child($1); } |
             ID DEL { driver.current = resource->Child($1); } path { $$ = $4; };

path : %empty { $$ = current->Value(); } |
       DEL path {} | CUR path {} |
       PAR path { driver.current = driver.current->Parent(); } |
       ID path  {
         auto parent = std::dynamic_pointer_cast<Resources::Body::ParentResource>(driver.current);
         if (parent != nullptr) driver.current = parent->Child($1);
         else YYABORT;
       } |
       ID key_list path {
         auto parent = std::dynamic_pointer_cast<Resources::Body::ParentResource>(driver.current);
         if (parent != nullptr) {
           driver.current = parent->Child($1);
           auto list = std::dynamic_pointer_cast<Resources::Body::ListResource>(driver.current);
           if (list != nullptr) {
             if (!list->ValidateKeys($2)) YYABORT;
           }
         }
         else YYABORT;
       } |
       ID nkey_list path;

key_list : key_list keys {
             $1.insert(std::end($1),
                       std::make_move_iterator(std::begin($2)),
                       std::make_move_iterator(std::end($2)));
             $$ = $1;
           } | keys { $$ = $1; };

keys : SO key SC { $$ = std::move($2); };

key : ID EQ SQSTR {
        std::vector<std::pair<std::string, std::string>> r;
        r.emplace_back($1, $3.substr(1, $3.length() - 2));
        $$ = r;
      };

nkey_list : nkey_list nkeys | nkeys;

nkeys : SO INT SC;

%%

void polycube::polycubed::Rest::Parser::XPathParser::error(
    const Parser::location_type& l, const std::string& m) {
    driver.error(l, m);
}