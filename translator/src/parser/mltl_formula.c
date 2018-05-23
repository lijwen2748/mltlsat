/* 
 * 
 * File:   mtl_formula.h
 * Author: Jianwen Li
 * Interface for MTL formulas
 *
 * Created on November 6, 2017
 */

#include "mltl_formula.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * dynamic memory manuplation
 * @return 
 */
static mtl_formula *
allocate_mtl ()
{
  mtl_formula *ret = (mtl_formula *) malloc (sizeof *ret);

  if (ret == NULL)
    { //fail
      fprintf (stderr, "Memory error\n");
      exit (1);
      return NULL;
    }

  memset (ret, 0, sizeof (mtl_formula));

  return ret;
}

/**
 * dynamic create string
 * @param str
 * @return 
 */
static char *
allocate_cstr (const char *str)
{
  char *s = (char *) malloc ((strlen (str) + 1) * sizeof (char));

  if (s == NULL)
    { //fail
      fprintf (stderr, "Memory error\n");
      exit (1);
      return NULL;
    }

  strcpy (s, str);
  return s;
}

/**
 * dynamic create interval
 * @return 
 */
static interval *
allocate_interval ()
{
  interval *ret = (interval *) malloc (sizeof (*ret));

  if (ret == NULL)
    { //fail
      fprintf (stderr, "Memory error\n");
      exit (1);
      return NULL;
    }

  memset (ret, 0, sizeof (interval));
  return ret;
}


/**
 * create VAR
 * @param var 变量名
 * @return 
 */
mtl_formula *
create_var (const char *var)
{
  mtl_formula *ret = allocate_mtl ();

  ret->_type = eLITERAL;
  ret->_var = allocate_cstr (var);

  return ret;
}

/**
 * create interval
 * @param left, right
 * @return 
 */
interval *
create_interval (const int left, const int right)
{
  interval *ret = allocate_interval ();

  ret->_left = left;
  ret->_right = right;

  return ret;
}

/**
 * create formula
 * @param type 
 * @param left
 * @param right
 * @return 
 */
mtl_formula *
create_operation (EOperationType type, mtl_formula *left, mtl_formula *right, interval *inter)
{
  mtl_formula *ret = allocate_mtl ();

  ret->_type = type;
  ret->_left = left;
  ret->_right = right;
  ret->_inter = inter;

  return ret;
}

/**
 * print formula
 * @param root
 */
void
print_formula (mtl_formula *root)
{
  if (root == NULL) return;
  if (root->_var != NULL)
    printf ("%s", root->_var);
  else if (root->_type == eTRUE)
    printf ("true");
  else if (root->_type == eFALSE)
    printf ("false");
  else
    {
      printf ("(");
      print_formula (root->_left);
      switch (root->_type)
        {
        case eNOT:
          printf ("! ");
          break;
        case eGLOBALLY:
          printf ("G");
          printf ("[%d, %d] ", root->_inter->_left, root->_inter->_right);
          break;
        case eFUTURE:
          printf ("F");
          printf ("[%d, %d] ", root->_inter->_left, root->_inter->_right);
          break;
        case eUNTIL:
          printf (" U");
          printf ("[%d, %d] ", root->_inter->_left, root->_inter->_right);
          break;
        case eRELEASE:
          printf (" R");
          printf ("[%d, %d] ", root->_inter->_left, root->_inter->_right);
          break;
        case eWUNTIL:
          printf (" W");
          printf ("[%d, %d] ", root->_inter->_left, root->_inter->_right);
          break;
        case eWRELEASE:
          printf (" V");
          printf ("[%d, %d] ", root->_inter->_left, root->_inter->_right);
          break;
        case eAND:
          printf (" & ");
          break;
        case eOR:
          printf (" | ");
          break;
        case eIMPLIES:
          printf (" -> ");
          break;
        case eEQUIV:
          printf (" <-> ");
          break;
        default:
          fprintf (stderr, "Error formula!");
        }
      print_formula (root->_right);
      printf (")");
    }
}

/**
 * destroy formula
 * @param root
 */
void
destroy_formula (mtl_formula *root)
{
  if (root == NULL) return;

  destroy_formula (root->_left);
  destroy_formula (root->_right);

  destroy_node (root);
}

/**
 * destroy node with the non-recursive way
 * @param node
 */
void
destroy_node (mtl_formula *node)
{
  if (node->_var != NULL) free (node->_var), node->_var = NULL;
  free (node), node = NULL;
}
