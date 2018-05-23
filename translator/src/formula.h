/* 
 * File:   formula.h
 * Author: Jianwen Li
 *
 * Created on November 16, 2017
 */

#ifndef FORMULA_H
#define	FORMULA_H

#include "util/define.h"
#include "util/hash_map.h"
#include "util/hash_set.h"
#include "parser/mltl_formula.h"

#include <vector>
#include <string>
#include <set>


namespace mltl
{

struct Interval 
{
    int _left;
    int _right;
    
    Interval (int left, int right) : _left (left), _right (right) {}
};

class Formula
{


private:
  ////////////
  //members//
  //////////////////////////////////////////////////
  int _op; // operator
  Formula *_left; 
  Formula *_right; 
  size_t _hash; // hash value
  Formula *_unique; // to the unique pointer
  Formula *_simp; // unique pointer after simplification
  
  Interval* _int;
  //////////////////////////////////////////////////

public:
 

  /* hash value for formula */
  struct mf_hash
  {

    size_t operator () (const Formula& mf) const
    {
      return mf._hash;
    }
  };

  /* hash value for pointer */
  struct mf_prt_hash
  {

    size_t operator () (const Formula *mf_prt) const
    {
      return mf_prt->_id;
    }
  };

  /* another hash value for pointer */
  struct mf_prt_hash2
  {

    size_t operator () (const Formula *mf_prt) const
    {
      return mf_prt->_hash;
    }
  };
  /* pointer equal */
  struct mf_prt_eq
  {

    bool operator () (const Formula *mf_prt1, const Formula *mf_prt2) const
    {
      return *mf_prt1 == *mf_prt2;
    }
  };
  
  /*
  inline bool interval_equal (const Interval* int1, const Interval* int2)
  {
    if (int1 == NULL)
    {
        if (int2 != NULL)
            return false;
        else
            return true;
    }
    else
    {
        if (int2 == NULL)
            return false;
        else
            return (int1->_left == int2->_left) && (int1->_right == int2->_right);
    }
  }
  */


public:
  typedef hash_set<Formula *, mf_prt_hash2, mf_prt_eq> mfp_set;
  typedef hash_set<Formula *, mf_prt_hash> mf_prt_set;


  //////////////
  //static members//
  //////////////////////////////////////////////////
private:
  static std::vector<std::string> names; // vars
  static hash_map<std::string, int> ids; // map from var to id
  
  static mfp_set all_mfs;   // all Formula object's pointer
  static Formula *_TRUE;
  static Formula *_FALSE;
  //////////////////////////////////////////////////

public:

  /* 操作符类型 */
  enum opkind
  {
    True,
    False,
    Literal,
    Not,
    Or,
    And,
    Next,
    Weak_Next,
    Until,
    Release,
    Weak_Until,  //Dual of Release
    Weak_Release,  //Dual of Until
    Undefined
  };

public:
  Formula ();
  Formula (const char *input);
  
  Formula (const Formula& orig);
  Formula (const mtl_formula *formula, bool is_not = false);
  Formula (int op, Formula *left, Formula *right, Interval *interval=NULL);

  Formula* nnf ();
  Formula* nnf_not ();

  virtual ~Formula ();

  Formula& operator = (const Formula& mf);
  bool operator == (const Formula& mf)const;

  int oper ()const;
  Formula *l_mf ()const;
  Formula *r_mf ()const;
  Formula *clone ()const;
  std::string to_string ()const;
  std::string to_RPN ()const;

  Formula *unique ();
  inline Interval *interval () {return _int;}
  inline size_t hash () {return _hash;} 
  inline int id () {return _id;}
  Formula *decrease_interval ();

private:

  //added for mf_prt_set identification, _id is set in unique ()
  int _id;
  static int _max_id;
  //

  void init ();
  void clc_hash ();

  void build (const mtl_formula *formula, bool is_not = false);
  void build_atom (const char *name, bool is_not = false);


public:
  static std::string get_name (int index);
  static void destroy ();
  static Formula *TRUE();
  static Formula *FALSE();

};

}

#endif	/* FORMULA_H */

