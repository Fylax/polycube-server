%{
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <memory>

#include <utility>
#include "../../include/Resources/CubeManager.h"
#include "../../include/Resources/Body/Resource.h"
#include "../../include/Resources/Body/LeafResource.h"
#include "../../include/Resources/Body/LeafListResource.h"
#include "../../include/Resources/Body/ParentResource.h"
#include "../../include/Resources/Body/ListResource.h"
%}

%require "3.0"
%skeleton "lalr1.cc"
%define api.namespace {polycube::polycubed::Rest::Parser}
%define parser_class_name {XPathParser}
%defines
%language "C++"
%no-lines

%code requires{
#include <cstdint>
#include <unordered_map>
namespace polycube::polycubed::Rest::Parser {
  class XPathParserDriver;
}
}


%define api.value.type variant
%define parse.assert

%parse-param {XPathParserDriver &driver}

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

%type <std::unordered_map<std::string, std::string>> key;
%type <std::unordered_map<std::string, std::string>> keys;
%type <std::unordered_map<std::string, std::string>> key_list;

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

start_path : DEL ID DEL ID {
               driver.current = Resources::CubeManager::GetInstance().Cube($2, $4);
             } path { $$ = $6; } |
             CUR {
               auto leaf = std::dynamic_pointer_cast<Resources::Body::LeafResource>(driver.current);
               $$ = leaf->Value();
             } |
             CUR DEL path { $$ = $3; } |
             PAR { YYABORT; } |
             PAR DEL { driver.current = driver.current->Parent(); } path { $$ = $4; } |
             ID {
               auto parent = std::dynamic_pointer_cast<Resources::Body::ParentResource>(driver.current);
               if (parent != nullptr) driver.current = parent->Child($1);
               else YYABORT;
             } |
             ID DEL {
               auto parent = std::dynamic_pointer_cast<Resources::Body::ParentResource>(driver.current);
               if (parent != nullptr) driver.current = parent->Child($1);
               else YYABORT;
             } path { $$ = $4; };

path : %empty {
         auto leaf = std::dynamic_pointer_cast<Resources::Body::LeafResource>(driver.current);
         if (leaf != nullptr) $$ = leaf->Value();
       } |
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
           } else {
             auto leaflist = std::dynamic_pointer_cast<Resources::Body::LeafListResource>(driver.current);
             if (leaflist != nullptr) driver.current = leaflist->Entry(std::begin($2)->second);
             else YYABORT;
           }
         } else YYABORT;
       } |
       ID nkey_list path;

key_list : key_list keys {
             $1.merge($2);
             $$ = $1;
           } | keys { $$ = $1; };

keys : SO key SC { $$ = std::move($2); };

key : ID EQ SQSTR {
        std::unordered_map<std::string, std::string> r;
        r.emplace($1, $3.substr(1, $3.length() - 2));
        $$ = r;
      };

nkey_list : nkey_list nkeys | nkeys;

nkeys : SO INT SC;
