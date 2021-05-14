%option noyywrap
%option yylineno

%{
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

char *yyfilename = "yyin";
int yyfileindex = 1;

void
yyerror (char const *str)
{
  fprintf (stderr, "\tline %i: %s\n", yylineno, str);
  exit (2);
}
%}

%x COMMENT

SEPERATOR [\(\),.:;\[\]\{\}]

BOOLEAN   true|false
NATURAL   0|[1-9][0-9]*
INTEGER   ("+"|"-"){NATURAL}
REAL      ({NATURAL}|{INTEGER})"."[0-9]+
CHARACTER '(\\.|[^'\\])?'
STRING    \"(\\.|[^"\\])*\"

IDENTIFIER [A-Z_a-z][0-9A-Z_a-z]*

WHITESPACE [\n\r\t ]

%%

<INITIAL>"//".*  ;
<INITIAL>"/*"    BEGIN (COMMENT);
<COMMENT>"*/"    BEGIN (INITIAL);
<COMMENT>[^*\n]+ ;
<COMMENT>"*"     ;
<COMMENT>\n      ;

<INITIAL>{SEPERATOR} {
{
  return (yytext[0]);
}}

<INITIAL>"begin"     { return (CONTROL_BEGIN);   }
<INITIAL>"end"       { return (CONTROL_END);     }
<INITIAL>"if"        { return (CONTROL_IF);      }
<INITIAL>"else"      { return (CONTROL_ELSE);    }
<INITIAL>"switch"    { return (CONTROL_SWITCH);  }
<INITIAL>"case"      { return (CONTROL_CASE);    }
<INITIAL>"default"   { return (CONTROL_DEFAULT); }
<INITIAL>"while"     { return (CONTROL_WHILE);   }
<INITIAL>"until"     { return (CONTROL_UNTIL);   }
<INITIAL>"return"    { return (CONTROL_RETURN);  }
<INITIAL>"let"       { return (KEYWORD_LET);     }
<INITIAL>"boolean"   { return (TYPE_BOOLEAN);    }
<INITIAL>"natural"   { return (TYPE_NATURAL);    }
<INITIAL>"integer"   { return (TYPE_INTEGER);    }
<INITIAL>"real"      { return (TYPE_REAL);       }
<INITIAL>"character" { return (TYPE_CHARACTER);  }
<INITIAL>"string"    { return (TYPE_STRING);     }

<INITIAL>":=" { return (ASSIGNMENT); }

<INITIAL>{BOOLEAN} {
{
  yylval.LITERAL_BOOLEAN = strdup (yytext);
  return (LITERAL_BOOLEAN);
}}

<INITIAL>{NATURAL} {
{
  yylval.LITERAL_NATURAL= strdup (yytext);
  return (LITERAL_NATURAL);
}}

<INITIAL>{INTEGER} {
{
  yylval.LITERAL_INTEGER = strdup (yytext);
  return (LITERAL_INTEGER);
}}

<INITIAL>{REAL} {
{
  yylval.LITERAL_REAL = strdup (yytext);
  return (LITERAL_REAL);
}}

<INITIAL>{CHARACTER} {
{
  yylval.LITERAL_CHARACTER = strdup (yytext);
  return (LITERAL_CHARACTER);
}}

<INITIAL>{STRING} {
{
  yylval.LITERAL_STRING = strdup (yytext);
  return (LITERAL_STRING);
}}

<INITIAL>{IDENTIFIER} {
{
  yylval.IDENTIFIER = strdup (yytext);
  return (IDENTIFIER);
}}

<INITIAL>{WHITESPACE} ;

<INITIAL>. {
{
  fprintf (stderr, "unexpected %s\n", yytext);
  exit (2);
}}

%%