%define parse.error verbose

%{
#include <stdio.h>
#include <stdlib.h>

#include "parser.tab.h"

extern FILE *yyin;
extern char *yyfilename;
extern int yyfileindex;
extern int yylineno;

int
yylex (void);

void
yyerror (char const *str);

#include "./include/context.h"

struct context *context;
%}

%define api.value.type union

%token CONTROL_BEGIN   "begin"
%token CONTROL_END     "end"
%token CONTROL_IF      "if"
%token CONTROL_ELSE    "else"
%token CONTROL_SWITCH  "switch"
%token CONTROL_CASE    "case"
%token CONTROL_DEFAULT "default"
%token CONTROL_DEFAULT "break"

%token CONTROL_WHILE   "while"
%token CONTROL_UNTIL   "until"

%token CONTROL_RETURN  "return"

%token KEYWORD_LET "let"

%token LOGICAL_NOT "not"
%token LOGICAL_AND "and"
%token LOGICAL_OR    "or"
%token LOGICAL_NAND "nand"
%token LOGICAL_NOR  "nor"
%token LOGICAL_XOR  "xor"
%token LOGICAL_XNOR "xnor"

%token <char *> LITERAL_BOOLEAN
%token <char *> LITERAL_NATURAL
%token <char *> LITERAL_INTEGER
%token <char *> LITERAL_REAL
%token <char *> LITERAL_CHARACTER
%token <char *> LITERAL_STRING

%token TYPE_BOOLEAN   "boolean"
%token TYPE_NATURAL   "natural"
%token TYPE_INTEGER   "integer"
%token TYPE_REAL      "real"
%token TYPE_CHARACTER "character"
%token TYPE_STRING    "string"

%token <char *> IDENTIFIER

%token ASSIGNMENT ":="

%left '+' '-'
%left '*' '/' '%'

%%

program:
  program procedure
| %empty
;

procedure:
  IDENTIFIER '(' parameters_opt ')' "begin" statements "end"
;

parameters_opt:
  parameters_opt parameters
| %empty
;

parameters:
  parameters ',' parameter
| parameter
;

parameter:
  IDENTIFIER ':' type
  {
    free ($[IDENTIFIER]);
  }
| IDENTIFIER ":=" expression
  {
    free ($[IDENTIFIER]);
  }
;

type:
  type modifier
| "boolean"
| "natural"
| "integer"
| "real"
| "character"
| "string"
;

modifier:
  '[' ']'
| '{' '}'
| '(' ')'
;

literal:
  LITERAL_BOOLEAN
  {
    free ($[LITERAL_BOOLEAN]);
  }
| LITERAL_NATURAL
  {
    free ($[LITERAL_NATURAL]);
  }
| LITERAL_INTEGER
  {
    free ($[LITERAL_INTEGER]);
  }
| LITERAL_REAL
  {
    free ($[LITERAL_REAL]);
  }
| LITERAL_CHARACTER
  {
    free ($[LITERAL_CHARACTER]);
  }
| LITERAL_STRING
  {
    free ($[LITERAL_STRING]);
  }
;

statements:
  statements ';' statement 
| statement
;

statement:
  "let" parameters
| "if" expression "begin" statement clause "end"
| "while" expression "begin" statement clause "end"
| "until" expression "begin" statement clause "end"
| "return" expression
| %empty
;

clause:
  "else" statement
  {
    /* ... */
  }
| %empty
;

expression:
  identifiers
| literal
| expression '+' expression { /* ... */ }
| expression '-' expression { /* ... */ }
| expression '*' expression { /* ... */ }
| expression '/' expression { /* ... */ }
| expression '%' expression { /* ... */ }
;

identifiers:
  identifiers '.' IDENTIFIER
  {
    free ($[IDENTIFIER]);
  }
| IDENTIFIER
  {
    free ($[IDENTIFIER]);
  }
;

%%

int
main (int argc, char *argv[])
{
  if ((context = ctxalloc ()) == NULL) /* allocate the context */
    {
      fprintf (stdout, "unable to allocate context!\n");
      
      return (EXIT_FAILURE);
    }

  if (argc == 1)
    {
      while (yyparse () != 0)
        ;
      
      fprintf (stdout, "\tstdin [correct]\n");
    }
  else
    {
      for (argc--, argv++; argc--; argv++, yyfileindex++)
        {
          yyin = fopen (*argv, "r");

          if (yyin == NULL)
            {
              fprintf (stderr, "unable to open %s!\n", *argv);

              return (EXIT_FAILURE);
            }

          ctxreset (context); /* reset the context */

          while (yyparse () != 0)
            ;

          fprintf (stdout, "\t%i. %s [correct]\n", yyfileindex, *argv);
          fclose (yyin);
        }
    }
  
  ctxfree (context); /* free the context */

  return (EXIT_SUCCESS);
}
