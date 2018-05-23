/* 
 * parser function
 * File:   trans.h
 * Author: Jianwen Li
 *
 * Created on November 6, 2017
 */

#ifndef TRANS_H
#define	TRANS_H

#include "mltl_formula.h"
#include "mltlparser.h"
#include "mltllexer.h"

#ifdef	__cplusplus
extern "C"
{
#endif

  mtl_formula *getAST (const char *input);

#ifdef	__cplusplus
}
#endif

#endif	/* TRANS_H */

