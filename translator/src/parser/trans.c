/* 
 * parser function
 * File:   trans.h
 * Author: Jianwen Li
 *
 * Created on November 6, 2017
 */
 
#include "trans.h"

#include <stdlib.h>

mtl_formula *
getAST (const char *input)
{
  mtl_formula *formula;
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (yylex_init (&scanner))
    {
      // couldn't initialize
      return NULL;
    }

  state = yy_scan_string (input, scanner);
  if (yyparse (&formula, scanner))
    {
      // error parsing
      return NULL;
    }

  yy_delete_buffer (state, scanner);
  yylex_destroy (scanner);

  return formula;
}
