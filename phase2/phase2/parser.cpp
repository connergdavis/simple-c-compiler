/*
 * Author:  Conner Davis
 * File:	  parser.h
 */

# include <cctype>
# include <iostream>

# include "lexer.h"
# include "tokens.h"
# include "parser.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::to_string;

/**
 * The next token's type, whose possible values are found in 
 * tokens.h.
 * 
 * When the input has been fully read, the token is "DONE".
 */
static int lookahead;
/**
 * The next token's actual value. For example, tokens of type
 * STRING will have a value "contained within the quotes".
 */
static string buffer;

static void error( string src, string msg )
{
  report("Syntax error at token [ %s ]", buffer);
  report("Error source --> <%s()> ]", src);
  report("Error message --> %s", msg);
  exit(EXIT_FAILURE);
}

static string keep( int token )
{
  string buf = buffer;
  match(token);

  return buf;
}

static void match( int token )
{
  if (lookahead == token)
    lookahead = lexan(buffer);
  else
    error("match", string("token mismatch - expected <") +
      to_string(token) + "> found <" + to_string(lookahead) + ">"
    );
}

static void print( string output )
{
  cout << output << endl;
}

static bool is_specifier( )
{
  return lookahead == INT || lookahead == LONG || lookahead == STRUCT;
}

static void expression( )
{
  logical_cmp_expression();

  while (lookahead == OR)
  {
    match(OR);
    logical_cmp_expression();
    print("or");
  }
}

static void logical_cmp_expression( )
{
  equality_expression();

  while (lookahead == AND)
  {
    match(AND);
    equality_expression();
    print("and");
  }
}

static void equality_expression( )
{
  relation_expression();

  while (true)
  {
    if (lookahead == EQL)
    {
      match(EQL);
      relation_expression();
      print("eql");
    }
    else if (lookahead == NEQ)
    {
      match(NEQ);
      relation_expression();
      print("neq");
    }
    else break;
  }
}

static void relation_expression( )
{
  add_expression();

  while (true)
  {
    if (lookahead == LEQ)
    {
      match(LEQ);
      add_expression();
      print("leq");
    }
    else if (lookahead == GEQ)
    {
      match(GEQ);
      add_expression();
      print("geq");
    }
    else if (lookahead == '<')
    {
      match('<');
      add_expression();
      print("ltn");
    }
    else if (lookahead == '>')
    {
      match('>');
      add_expression();
      print("gtn");
    }
    else break;
  }
}

static void add_expression( )
{
  multiply_expression();

  while (true)
  {
    if (lookahead == '+')
    {
      match('+');
      multiply_expression();
      print("add");
    }
    else if (lookahead == '-')
    {
      match('-');
      multiply_expression();
      print("sub");
    }
    else break;
  }
}

static void multiply_expression( )
{
  prefix_expression();

  while (true)
  {
    if (lookahead == '*')
    {
      match('*');
      prefix_expression();
      print("mul");
    }
    else if (lookahead == '/')
    {
      match('/');
      prefix_expression();
      print("div");
    }
    else if (lookahead == '%')
    {
      match('%');
      prefix_expression();
      print("rem");
    }
    else break;
  }
}

static void prefix_expression( )
{
  if (lookahead == '!')
  {
    match('!');
    prefix_expression();
    print("not");
  }
  else if (lookahead == '-')
  {
    match('-');
    post_expression();
    print("neg");
  }
  else if (lookahead == '&')
  {
    match('&');
    post_expression();
    print("addr");
  }
  else if (lookahead == '*')
  {
    match('*');
    post_expression();
    print("deref");
  }
  else if (lookahead == SIZEOF)
  {
    match(SIZEOF);

    if (lookahead == '(')
    {
      match('(');
      prefix_expression();
      match(')');
    }

    print("sizeof");
  }
  else post_expression();
}

static void post_expression( )
{
  cast_expression();

  while (true)
  {
    if (lookahead == '[')
    {
      match('[');
      expression();
      match(']');
      print("index");
    }
    else if (lookahead == '.')
    {
      match('.');
      match(ID);
      print("dot");
    }
    else if (lookahead == '-')
    {
      match('-');
      match('>');
      match(ID);
      print("arrow");
    }
    else break;
  }
}

static void cast_expression( )
{
  if (lookahead == '(')
  {
    match('(');

    if (is_specifier())
    {
      if (lookahead == INT) match(INT);
      else if (lookahead == LONG) match(LONG);
      else if (lookahead == STRUCT) match(STRUCT);

      //pointers();
      match(')');
      expression();
      print("cast");
    }
    else
    {
      expression();
      match(')');
    }
  } else general_expression();
}

static void general_expression( )
{
  if (lookahead == NUM)
  {
    match(NUM);
  }
  else if (lookahead == ID)
  {
    match(ID);
    
    if (lookahead == '(')
    {
      match('(');
      
      if (lookahead != ')')
      {
        //argument_list();
      }

      match(')');
    }
  }
  else error("general_expression", "invalid lookahead");
}

int main( void )
{
  // read the first token
  lookahead = lexan(buffer);

  // read tokens until end of input stream
  while (lookahead != DONE)
  {
    expression();
  }

  return EXIT_SUCCESS;
}
