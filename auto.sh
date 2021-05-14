#!/bin/bash

readonly INCLUDE="./include"
readonly SRC="./src"

function require ()
{
  for arg in "$@"; do
    if ! [ -x "$(command -v $arg)" ]; then
      echo "$arg is not installed!" >&2
      exit 1
    fi
  done
}

function attempt ()
{
  "$@"

  if [ $? -ne 0 ]; then
    exit 1
  fi
}

function main ()
{
  # the required programs
  require bison flex gcc g++

  # generate the parser and lexer
  attempt bison -d -Wcounterexamples "$SRC/parser.y"
  attempt flex "$SRC/lexer.l"

  # the (f)lex library
  declare LIBRARY

  # set said (f)lex library
  case "$(uname -s)" in
    "Darwin")
    readonly LIBRARY=-ll
    ;;
    *)
    readonly LIBRARY=-lfl
    ;;
  esac

  # compile the translator
  attempt gcc -std=c99 *.c $SRC/*.c $LEX -o zed

  # clean up files
  rm lex.yy.c parser.tab.c parser.tab.h

  # test the translator
  attempt ./zed example.zeta
}

main