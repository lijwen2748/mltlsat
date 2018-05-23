/* 
 * MTL parser tester
 * File:   main.c
 * Author: Jianwen
 *
 * Created on November 6, 2017
 */

#include "mltl_formula.h"
#include "trans.h"

#include <stdio.h>
#include <stdlib.h>

#define MAXN 10000

char in[MAXN];

int
main (int argc, char** argv)
{
  while (fgets (in, sizeof (in), stdin))
    {
      mtl_formula *formula = getAST(in);
      print_formula(formula);
      puts("");
      destroy_formula(formula);
    }

  return (EXIT_SUCCESS);
}

