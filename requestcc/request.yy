%skeleton "lalr1.cc"
%require  "3.2"
%language "c++"

%define parse.trace
%define parse.error verbose

%defines "requestyy.hpp"
%output "requestyy.cpp"

%define api.namespace { requestcc }
%define api.location.file "requestloc.hpp"
%locations

%code requires{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <memory>

    #include "requestcc.hpp"

    namespace requestcc {
            class lexer;
    }
}


%parse-param {requestcc::lexer& lex} {srv6::request& req}


%code{
    #include "requestll.hpp"

    #undef yylex
    #define yylex lex.yylex

    std::string tolower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){
                return std::tolower(c);
        });
        return s;
    }
}

%code requires {
    struct lexdata {
        std::string str;
    };
}
%define api.value.type {struct lexdata}

%token <str> GET
%token <str> URI
%token PROTO

%token HOST
%token <str> HNAME
%token <str> PORT

%token AUTH
%token <str> AMET
%token <str> AKEY

%token CLEN
%token <str> CSIZE

%token SEMIC
%token SPACE
%token NL
%token END


%%

assignments : assignment
            | assignment assignments
            ;
assignment  : some NL
            ;

some        : req
            | host
            | auth
            | clen
            | error
            ;
req         : GET SPACE URI SPACE PROTO
                        { req.method = tolower($1); req.uri = $3; }
            ;

host        : HOST SPACE HNAME SEMIC PORT
                        { req.hostname = tolower($3); }
            ;

auth        : AUTH SPACE AMET SPACE AKEY
                        { req.auth.method = tolower($3); req.auth.key = $5; }
            ;

clen        : CLEN SPACE CSIZE
                        {   std::string clen($3);
                            if (clen.length() > 0) {
                                req.clen = std::stoi(clen);
                            } else {
                                req.clen = 0;
                            }
                        }
            ;
%%
void requestcc::parser::error(const location_type& loc, const std::string& msg) {
    std::cerr << msg << " at " << loc << std::endl;
}
