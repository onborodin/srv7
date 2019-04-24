%top{
#include "requestyy.hpp"
%}

%option bison-cc
%option bison-locations
%option noyywrap

%option namespace="requestcc"
%option lexer="lexer"

%option lex="yylex"
//%outfile="requestll.cpp"
%header-file="requestll.hpp"
%option bison-cc-namespace="requestcc"

%x GET
%x AUTH
%x HOST
%x CLEN

GET         GET|POST|HEAD
URI             \/[\/a-zA-Z0-9_%&?=.]{0,1024}
PROTO           HTTP\/1.0|HTTP\/1.1

HOST            (?i:host):
HNAME           [a-zA-Z_][a-zA-Z0-9\._]{0,512}
PORT            [1-6][0-9]{1,4}

AUTH           (?i:authorization):
AMET           (?i:SAPISIDHASH|BASIC)
AKEY           [a-zA-Z_][a-zA-Z0-9_=]{1,1024}

CLEN            (?i:content\-length):
CSIZE           [1-9][0-9]{1,64}

SEMIC           :
NL              [\t ]{0,64}\n|[\t ]{0,64}\r\n
SPACE           [\t ]{1,64}

%%

{GET}            { start(GET); yylval.string = strdup(text()); return requestcc::parser::token::GET; }
<GET>{SPACE}         { return requestcc::parser::token::SPACE; }
<GET>{URI}           { yylval.string = strdup(text()); return requestcc::parser::token::URI; }
<GET>{PROTO}         { return requestcc::parser::token::PROTO; }
<GET>{NL}            { start(INITIAL); return requestcc::parser::token::NL; }

{HOST}            { start(HOST); return requestcc::parser::token::HOST; }
<HOST>{HNAME}         { yylval.string = strdup(text()); return requestcc::parser::token::HNAME; }
<HOST>{PORT}          { yylval.string = strdup(text()); return requestcc::parser::token::PORT; }
<HOST>{SPACE}         { return requestcc::parser::token::SPACE; }
<HOST>{SEMIC}         { return requestcc::parser::token::SEMIC; }
<HOST>{NL}            { start(INITIAL); return requestcc::parser::token::NL; }

{AUTH}            { start(AUTH); return requestcc::parser::token::AUTH; }
<AUTH>{AMET}         { yylval.string = strdup(text()); return requestcc::parser::token::AMET; }
<AUTH>{AKEY}         { yylval.string = strdup(text()); return requestcc::parser::token::AKEY; }
<AUTH>{SPACE}        { return requestcc::parser::token::SPACE; }
<AUTH>{NL}           { start(INITIAL); return requestcc::parser::token::NL; }

{CLEN}            { start(CLEN); return requestcc::parser::token::CLEN; }
<CLEN>{CSIZE}         { yylval.string = strdup(text()); return requestcc::parser::token::CSIZE; }
<CLEN>{SPACE}         { return requestcc::parser::token::SPACE; }
<CLEN>{NL}            { start(INITIAL); return requestcc::parser::token::NL; }


%%
