# CIS 3150 Assignment 3

Name: Kyle Lukaszek

ID: 1113798

Due: November 8th, 2023

## Compilation

To compile the program, run the following command:

```bash
make
```

## Usage

To run the program, run the following command:

```bash
./a3 <input_file>
```

The program reads regex from all lines in the input file and then displays a tree of the regex given the context free grammar for regex.

## Description

The CFG for regex is as follows:

```
<regexp> ::= <concat>
<concat> ::= <term><concat> | <term> | <endofline>
<term> ::= <star> | <element> 
<star> ::= <element>*
<element> ::= <group> | <char>
<group> ::= (<regexp>)
<char> ::= <alphanum> | <symbol> | <white> 
<alphanum> ::= A | B | C | ... | Z |
               a | b | c | ... | z |
               0 | 1 | 2 | ... | 9
<symbol> ::= ! | " | # | $ | % | & | ' | + | , | - | . | 
             / | : | ; | < | = | > | ? | @ | [ | ] | ^ | 
             _ | ` | { | } | ~ | <sp> | <metachars>
<sp> ::= " "
<metachar> ::= \ | "|" |  <white>
<white> ::= <tab> | <vtab> | <nline>
<tab> ::= '\t'
<vtab> ::= '\v'
<nline> ::= '\n'
<endofline> ::= '\0'
```

The program assumes that the regex cannot be more than 1000 characters long.

When a terminal matches a character in the regex string, the tree will print "match" or "fail" at the bottom of a branch.

## Example

./a3 file

file regex = "(a)"

Output:
```
regexp
concat
term
star
element
group
regexp
concat
term                                    concat                                                                                    term
star                element             term                                    term                                    eoln      star                element
element             group     char      star                element             star                element             fail      element             group
group     char      fail      match     element             group     char      element             group     char                group     char      fail
fail      match                         group     char      fail      fail      group     char      fail      fail                fail      match
                                        fail      fail                          fail      fail




          element
          group
          regexp
          concat
          term                                    concat                                                                                    term
          star                element             term                                    term                                    eoln      star
          element             group     char      star                element             star                element             fail      element
char      group     char      fail      match     element             group     char      element             group     char                group     char
match     fail      match                         group     char      fail      fail      group     char      fail      fail                fail      match
                                                  fail      fail                          fail      fail




                    concat
                    term                                    term                                    eoln
                    star                element             star                element             match
                    element             group     char      element             group     char
                    group     char      fail      fail      group     char      fail      fail
                    fail      fail                          fail      fail
element
group     char
fail      match
```