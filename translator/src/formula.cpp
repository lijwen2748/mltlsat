/* 
 * File:   Formula.cpp
 * Author: Jianwen Li
 * 
 * Created on November 16, 2017
  */

#include "formula.h"
#include "util/utility.h"
#include "parser/trans.h"

#include <string.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <zlib.h>
#include <assert.h>
using namespace std;

namespace mltl
{

///////////////////////////////////////////

std::vector<std::string> Formula::names;
hash_map<std::string, int> Formula::ids;
Formula::mfp_set Formula::all_mfs;
Formula *Formula::_TRUE = NULL;
Formula *Formula::_FALSE = NULL;



Formula *
Formula::TRUE ()
{
  if (_TRUE == NULL)
    {
      _TRUE = Formula (True, NULL, NULL).unique ();
      _TRUE->_unique = _TRUE;
    }
  return _TRUE;
}

Formula *
Formula::FALSE ()
{
  if (_FALSE == NULL)
    {
      _FALSE = Formula (False, NULL, NULL).unique ();
      _FALSE->_unique = _FALSE;
    }
  return _FALSE;
}

std::string
Formula::get_name (int index)
{
  return names[index];
}

void
Formula::destroy ()
{
  for (mfp_set::iterator it = all_mfs.begin (); it != all_mfs.end (); it ++)
  {
    if ((*it)->_int != NULL)
    {
        delete (*it)->_int;
        (*it)->_int = NULL;
    }
    delete (*it);
  }
    
  all_mfs.clear ();
  ids.clear ();
  names.clear ();
  _TRUE = NULL;
  _FALSE = NULL;
}

///////////////////////////////////////////


inline void
Formula::init ()
{
  _left = _right = _unique = NULL;
  _op = Undefined;
  _int = NULL;
  if (names.empty ())
    { //Be in order
      names.push_back ("true");
      names.push_back ("false");
      names.push_back ("Literal");
      names.push_back ("!");
      names.push_back ("|");
      names.push_back ("&");
      names.push_back ("X");
      names.push_back ("N");
      names.push_back ("U");
      names.push_back ("R");
      names.push_back ("W");  //Weak Until
      names.push_back ("V");  //Weak Release
      names.push_back ("Undefined");
    }
}

/**
 * 计算hash值
 */
inline void
Formula::clc_hash ()
{
  _hash = HASH_INIT;
  _hash = (_hash << 5) ^ (_hash >> 27) ^ _op;
  
  if (_left != NULL)
    _hash = (_hash << 5) ^ (_hash >> 27) ^ _left->_hash;
  if (_right != NULL)
    _hash = (_hash << 5) ^ (_hash >> 27) ^ _right->_hash;

  if (_int != NULL)
    _hash = (_hash << 5) ^ (_hash >> 27) ^ _int->_left ^ _int->_right;
}

Formula::Formula ()
{
  init ();
}

Formula::Formula (const Formula& orig)
{
  *this = orig;
}

Formula::Formula (int op, Formula *left, Formula *right, Interval* interval)
: _op (op), _unique (NULL)
{
   if (left == NULL)
     _left = NULL;
   else 
     _left = left->unique ();
   if (right == NULL)
     _right = NULL;
   else
     _right = right->unique ();
   _int = interval;
   clc_hash ();
  
}

Formula::Formula (const char *input)
{
  init ();
  mtl_formula *formula = getAST (input);
  build (formula);
  destroy_formula (formula);
  clc_hash ();
}

Formula::Formula (const mtl_formula *formula, bool is_not)
{
  init ();
  build (formula, is_not);
  clc_hash ();
}

Formula::~Formula () { }

Formula * 
Formula::nnf ()
{
  Formula *result, *l, *r;
  if (oper () == Not)
  {
    if (_right->oper () == Not)
      result = _right->_right->nnf ();
    else
      result = _right->nnf_not ();
  }
  else
  {
    l = NULL;
    r = NULL;
    if (_left != NULL)
      l = _left->nnf ();
    if (_right != NULL)
      r = _right->nnf ();
    result = Formula (oper (), l, r, _int).unique ();
  }
  return result;
}

Formula* 
Formula::nnf_not ()
{
  Formula *result, *l, *r;
  switch (oper ())
  {
    case True:
      result = FALSE ();
      break;
    case False:
      result = TRUE ();
      break;
    case Not:
      result = _right->nnf ();
      break;
    case And:
      l = _left->nnf_not ();
      r = _right->nnf_not ();
      result = Formula (Or, l, r, _int).unique ();
      break;
    case Or:
      l = _left->nnf_not ();
      r = _right->nnf_not ();
      result = Formula (And, l, r, _int).unique ();
      break;
    case Until:
      l = _left->nnf_not ();
      r = _right->nnf_not ();
      result = Formula (Weak_Release, l, r).unique ();
      break;
    case Release:
      l = _left->nnf_not ();
      r = _right->nnf_not ();
      result = Formula (Weak_Until, l, r).unique ();
      break;
    case Weak_Until:
      l = _left->nnf_not ();
      r = _right->nnf_not ();
      result = Formula (Release, l, r).unique ();
      break;
    case Weak_Release:
      l = _left->nnf_not ();
      r = _right->nnf_not ();
      result = Formula (Until, l, r).unique ();
      break;
    default:
      result = Formula (Not, NULL, this).unique ();
      break;
  }
  return result;
}


Formula& Formula::operator = (const Formula& mf)
{
  if (this != &mf)
    {
      this->_left = mf._left;
      this->_right = mf._right;
      this->_op = mf._op;
      this->_hash = mf._hash;
      this->_unique = mf._unique;
      this->_id = mf._id;
      this->_int = mf._int;
    }
  return *this;
}


bool Formula::operator == (const Formula& mf) const
{
    if (_left != mf._left || _right != mf._right || _op != mf._op)
        return false;
    if (_int == NULL && mf._int == NULL)
        return true;
    if (_int != NULL && mf._int != NULL)
    {
        return _int->_left == mf._int->_left && _int->_right == mf._int->_right;
    }
    
    return false;
}


void
Formula::build (const mtl_formula *formula, bool is_not)
{
  Formula *tmp_left, *tmp_right;
  if (formula == NULL) return;
  switch (formula->_type)
    {
    case eTRUE: // True - [! True = False]
      if (is_not) _op = False;
      else _op = True;
      break;
    case eFALSE: // False - [! False = True]
      if (is_not) _op = True;
      else _op = False;
      break;
    case eLITERAL: // 原子
      build_atom (formula->_var, is_not);
      break;
    case eNOT: // ! -- [!!a = a]
      build (formula->_right, is_not ^ 1); //complement is_not
      break;
    case eGLOBALLY: // G a = False R a -- [!(G a) = True U !a]
      if (is_not) _op = Weak_Until, _left = TRUE ();
      else _op = Release, _left = FALSE ();
      _right = Formula (formula->_right, is_not).unique ();
      _int = new Interval (formula->_inter->_left, formula->_inter->_right);
      break;
    case eFUTURE: // F a = True U a -- [!(F a) = False R !a]
      if (is_not) _op = Weak_Release, _left = FALSE ();
      else _op = Until, _left = TRUE ();
      _right = Formula (formula->_right, is_not).unique ();
      _int = new Interval (formula->_inter->_left, formula->_inter->_right);
      break;
    case eUNTIL: // a U b -- [!(a U b) = !a R !b]
      _op = is_not ? Weak_Release : Until;
      _left = Formula (formula->_left, is_not).unique ();
      _right = Formula (formula->_right, is_not).unique ();
      assert (formula->_inter != NULL);
      _int = new Interval (formula->_inter->_left, formula->_inter->_right);
      break;
    case eRELEASE: // a R b -- [!(a R b) = !a U !b]
      _op = is_not ? Weak_Until : Release;
      _left = Formula (formula->_left, is_not).unique ();
      _right = Formula (formula->_right, is_not).unique ();
      _int = new Interval (formula->_inter->_left, formula->_inter->_right);
      break;
    case eWUNTIL: // a W b -- [!(a W b) = !a R !b]
      _op = is_not ? Release : Weak_Until;
      _left = Formula (formula->_left, is_not).unique ();
      _right = Formula (formula->_right, is_not).unique ();
      _int = new Interval (formula->_inter->_left, formula->_inter->_right);
      break;
    case eWRELEASE: // a V b -- [!(a V b) = !a U !b]
      _op = is_not ? Until : Weak_Release;
      _left = Formula (formula->_left, is_not).unique ();
      _right = Formula (formula->_right, is_not).unique ();
      _int = new Interval (formula->_inter->_left, formula->_inter->_right);
      break;
    case eAND: // a & b -- [!(a & b) = !a | !b ]
      _op = is_not ? Or : And;
      _left = Formula (formula->_left, is_not).unique ();
      _right = Formula (formula->_right, is_not).unique ();
      break;
    case eOR: // a | b -- [!(a | b) = !a & !b]
      _op = is_not ? And : Or;
      _left = Formula (formula->_left, is_not).unique ();
      _right = Formula (formula->_right, is_not).unique ();
      break;
    case eIMPLIES: // a->b = !a | b -- [!(a->b) = a & !b]
      _op = is_not ? And : Or;
      _left = Formula (formula->_left, is_not ^ 1).unique ();
      _right = Formula (formula->_right, is_not).unique ();
      break;
    case eEQUIV:
      { // a<->b = (!a | b)&(!b | a) -- [!(a<->b) = (a & !b)|(!a & b)]
        mtl_formula *not_a = create_operation (eNOT, NULL, formula->_left);
        mtl_formula *not_b = create_operation (eNOT, NULL, formula->_right);
        mtl_formula *new_left = create_operation (eOR, not_a, formula->_right);
        mtl_formula *new_right = create_operation (eOR, not_b, formula->_left);
        mtl_formula *now = create_operation (eAND, new_left, new_right);
        *this = *(Formula (now, is_not).unique ());
        destroy_node (not_a);
        destroy_node (not_b);
        destroy_node (new_left);
        destroy_node (new_right);
        destroy_node (now);
        break;
      }
    default:
      print_error ("the formula cannot be recognized!");
      exit (1);
      break;
    }
}



void
Formula::build_atom (const char *name, bool is_not)
{
  int id;
  hash_map<std::string, int>::const_iterator it = ids.find (name);
  if (it == ids.end ())
    { 
      id = names.size ();
      ids[name] = id;
      names.push_back (name);
    }
  else
    {
      id = it->second;
    }
  if (is_not) 
    _op = Not, _right = Formula (id, NULL, NULL).unique ();
  else 
    _op = id;
}





int
Formula::oper () const
{
  return _op;
}


Formula *
Formula::l_mf () const
{
  return _left;
}


Formula *
Formula::r_mf () const
{
  return _right;
}



Formula *
Formula::clone () const
{
  Formula *new_formula = new Formula (*this);
  if (new_formula == NULL)
    {
      destroy ();
      print_error ("Memory error!");
      exit (1);
    }
  if (_int != NULL)
    new_formula->_int = new Interval (_int->_left, _int->_right);
  
  return new_formula;
}

int Formula::_max_id = 1;


Formula *
Formula::unique ()
{
  if (_unique == NULL)
    {
      mfp_set::const_iterator iter = all_mfs.find (this);
      if (iter != all_mfs.end ())
        _unique = (*iter);
      else
        {
        
          _unique = clone ();
          _unique->_id = _max_id ++;
          all_mfs.insert (_unique);
          _unique->_unique = _unique;
        }
    }
  return _unique;
}

Formula * Formula::decrease_interval ()
{
    assert (_int != NULL);
    Formula* res = clone ();
    //set res->unique_ = NULL;
    res->_unique = NULL;
    
    if (res->_int->_left > 0)
    {
        res->_int->_left -= 1;
        res->_int->_right -= 1;
        //special case when [_left = _right = 0]
        if (res->_int->_left == 0 && res->_int->_right == 0) {
            delete res;
            return r_mf ();
        }
    }
    else if (res->_int->_right > 0)
    {
        if (res->_int->_right != 1)
            res->_int->_right -= 1;
        else
        {
            delete res;
            return r_mf ();
        }
    }
    else 
    {
        cout << res->_int->_left << " " << res->_int->_right << " decrease error\n";
        exit (0);
    }
    return res->unique ();
}


std::string
Formula::to_RPN () const
{
  if (_left == NULL && _right == NULL)
    return names[_op];
  if (_left != NULL && _right != NULL)
    return names[_op] + " " + _left->to_RPN () + " " + _right->to_RPN ();
  return names[_op] + " " + (_left == NULL ? _right->to_RPN () : _left->to_RPN ());
}

/**
 * To String
 * @return 
 */
std::string
Formula::to_string () const
{
  string interval = (_int == NULL) ? "" : "[" + int2str (_int->_left) + ", " + int2str (_int->_right) + "]";
  if (_left == NULL && _right == NULL)
    return names[_op];
  if (_left == NULL)
    return "(" + names[_op] + interval + " " + _right->to_string () + ")";
  if (_right == NULL)
    return "(" + _left->to_string () + " " + names[_op] + interval + ")";
  return "(" + _left->to_string () + " " + names[_op] + interval + " " + _right->to_string () + ")";
}


}













