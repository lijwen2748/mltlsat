/* 
 * 
 * File:   mtl_formula.h
 * Author: Jianwen Li
 * Interface for MTL formulas
 *
 * Created on November 6, 2017
 */

#ifndef MLTL_FORMULA_H
#define	MLTL_FORMULA_H

#include <stdio.h>

#ifdef	__cplusplus
extern "C"
{
#endif

#define INF -1

  /**
   * 操作符类型
   */
  typedef enum _OperationType
  {
    eUNDEFINED = 0,
    eTRUE,
    eFALSE,
    eLITERAL,
    eNOT,
    eGLOBALLY,
    eFUTURE,
    eUNTIL,
    eRELEASE,
    eWUNTIL,
    eWRELEASE,
    eAND,
    eOR,
    eIMPLIES,
    eEQUIV
  } EOperationType;

  /**
   * data structure for mtl formula
   */
  typedef struct _interval
  {
    int _left;
    int _right;
  } interval;
  
  typedef struct _mtl_formula
  {
    EOperationType _type;
    struct _mtl_formula *_left;
    struct _mtl_formula *_right;
    struct _interval *_inter;

    char *_var;
  } mtl_formula;

  /**
   * create VAR
   * @param var 
   * @return 
   */
  mtl_formula *create_var (const char *var);
  
  /**
   * create interal
   * @param left, right
   * @return
   */
  interval *create_interval (const int left, const int right);


  /**
   * create formula
   * @param type 
   * @param left
   * @param right
   * @param inter
   * @return 
   */
  mtl_formula *create_operation (EOperationType type, mtl_formula *left, mtl_formula *right, interval *inter=NULL);

  /**
   * print formula
   * @param root
   */
  void print_formula (mtl_formula *root);

  /**
   * destroy formula
   * 
   * @param root
   */
  void destroy_formula (mtl_formula *root);

  /**
   * destroy node with the non-recursive way
   * @param node
   */
  void destroy_node (mtl_formula *node);

#ifdef	__cplusplus
}
#endif

#endif	/* MTL_FORMULA_H */

