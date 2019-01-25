/* 
 * File:   translator.cpp
 * Author: Jianwen Li
 *
 * Created on December 26, 2017
 */
 
 #include "formula.h"
 #include "translator.h"
 
 #include <assert.h>
 #include <iostream>
 #include <sstream>
 #include <cstdlib>
 using namespace std;
 
 namespace mltl
 {
 
    int Translator::id_ = 0;
    /*
    * Translation from MLTL to LTL or LTLf 
    * When \@finite is true, the output is an LTLf formula
    */
    string Translator::mltl2ltl (Formula *f, bool finite)
    {
        string next = finite ? "X" : "(! Tail) & X";
        string weak_next = finite ? "N" : "Tail | X";
        string res;
        Interval *I = f->interval ();
        if (I != NULL)
            assert (I->_right >= I->_left);
        switch (f->oper ())
        {
            case Formula::True:
            case Formula::False:
            case Formula::Literal:
                res = f->to_string ();
                break;
            case Formula::Not:
                res = "(! " + mltl2ltl (f->r_mf (), finite) +")";
                break;
            case Formula::Or:
                res = "(" + mltl2ltl (f->l_mf (), finite) + " | " + mltl2ltl (f->r_mf (), finite) + ")";
                break;
            case Formula::And:
                res = "(" + mltl2ltl (f->l_mf (), finite) + " & " + mltl2ltl (f->r_mf (), finite) + ")";
                break;
            case Formula::Weak_Until:
            case Formula::Until:
                assert (I != NULL);
                if (I->_left > 0)
                {
                    res = "(" + next + mltl2ltl (f->decrease_interval (), finite) + ")";
                }
                else if (I->_left == 0)
                {
                    if (I->_right > 0)
                        res = "(" + mltl2ltl (f->r_mf (), finite) + " | (" + mltl2ltl (f->l_mf (), finite) + " & " + next + mltl2ltl (f->decrease_interval (), finite)+ "))";
                    else
                        res = mltl2ltl (f->r_mf (), finite);
                }
                else 
                {   
                    cout << "Error: Interval cannot be negative!\n";
                    exit (0);
                }
                break;
            case Formula::Weak_Release:
            case Formula::Release:
                assert (I != NULL);
                if (I->_left > 0)
                {
                    res = "(" + next + mltl2ltl (f->decrease_interval (), finite) + ")";
                }
                else if (I->_left == 0)
                {
                    if (I->_right > 0)
                        res = "(" + mltl2ltl (f->r_mf (), finite) + " & (" + mltl2ltl (f->l_mf (), finite) + " | " + next + mltl2ltl (f->decrease_interval (), finite)+ "))";
                    else
                        res = mltl2ltl (f->r_mf (), finite);
                }
                else 
                {   
                    cout << "Error: Interval cannot be negative!\n";
                    exit (0);
                }
                break;
            /*
            case Formula::Weak_Until:
                assert (I != NULL);
                if (I->_left > 0)
                {
                    res = "(" + weak_next  + mltl2ltl (f->decrease_interval (), finite) + ")";
                }
                else if (I->_left == 0)
                {
                    if (I->_right > 0)
                        res = "(" + mltl2ltl (f->r_mf (), finite) + " & (" + mltl2ltl (f->l_mf (), finite) + " | " + next + mltl2ltl (f->decrease_interval (), finite)+ "))";
                    else
                        res = mltl2ltl (f->r_mf (), finite);
                }
                else 
                {   
                    cout << "Error: Interval cannot be negative!\n";
                    exit (0);
                }
                break;
            case Formula::Weak_Release:
                assert (I != NULL);
                if (I->_left > 0)
                {
                    res = "(" + weak_next  + mltl2ltl (f->decrease_interval (), finite) + ")";
                }
                else if (I->_left == 0)
                {
                    if (I->_right > 0)
                        res = "(" + mltl2ltl (f->r_mf (), finite) + " | (" + mltl2ltl (f->l_mf (), finite) + " & " + next + mltl2ltl (f->decrease_interval (), finite)+ "))";
                    else
                        res = mltl2ltl (f->r_mf (), finite);
                }
                else 
                {   
                    cout << "Error: Interval cannot be negative!\n";
                    exit (0);
                }
                break;
            */
            default:
                res = f->to_string ();
                break;
        }
        return res;
    }
    
    /*
    * Translation from MLTL to SMV model
    */
    string Translator::mltl2smv (Formula* f)
    {
        string res = "MODULE main\n";
        f2s V, Def, Init, Tran, Invar;
        string strV = "VAR\n", strDef = "DEFINE\n", strInit = "INIT\n", strInvar = "INVAR\n", strTran = "TRANS\n";
        create_smv (f, V, strV, Def, strDef, Init, strInit, Invar, strInvar, Tran, strTran);
        strV += "\tTail : boolean;\n";
        strInvar += "\tTRUE;\n";
        strTran += "\tTRUE;\n";
        res += strV + strInit + strDef + strInvar + strTran;
        res += "LTLSPEC\n\tG (! Tail)\n";
        return res;
    }
    
    /*
    * Generate the set of variables for SMV model
    */
    string Translator::smv_var (Formula *f, f2s& V, string& strV)
    {
        string res = "";
        f2s::iterator it = V.find (f);
        if (it != V.end ())
            res = it->second;
        else
        {
            if (f->oper () > Formula::Undefined) //atoms
            {
                res = f->to_string ();
            }
            else if (f->oper () == Formula::True) {
                res = "TRUE";
            }
            else if (f->oper () == Formula::False) {
                res = "FALSE";
            } 
            else {
                
                ostringstream convert;
                convert << "S" << id_++;
                res = convert.str ();
            }
            
            V.insert (std::pair<Formula*, string> (f, res));
            if (f->oper () > Formula::Undefined || f->oper () == Formula::Next || f->oper () == Formula::Weak_Next)
                strV += "\t" + res + " : boolean;\n";
            //special case that [0,1]
            if (f->interval () != NULL) {
                if (f->interval()->_left == 0 && f->interval()->_right == 1) {
                    strV += "\t" + res + " : boolean;\n";
                }
            }
        }
        return res;
    }
    
    /*
    * Generate an SMV expression for a subformula 
    * 
    */
    string Translator::smv_expr (Formula *f, f2s& V, string& strV)
    {
        string res = "";
        string var, var2, var3, var4;
  
        switch (f->oper ())
        {
            case Formula::True:
                res = "TRUE";
                break;
            case Formula::False:
                res = "FALSE";
                break;
            case Formula::Literal:
                res += "!" + smv_var (f->r_mf (), V, strV);
                break;
            case Formula::Not:
                res += "(!" + smv_expr (f->r_mf (), V, strV) + ")";
                break;
            case Formula::Or:
                res += "(" + smv_expr (f->l_mf (), V, strV) + " | " + smv_expr (f->r_mf (), V, strV) + ")";
                break;
            case Formula::And:
                res += "(" + smv_expr (f->l_mf (), V, strV) + " & " + smv_expr (f->r_mf (), V, strV) + ")";
                break;
            case Formula::Undefined:
                cout << "smv_expr error: cannot recognize the MLTL formula\n";
                exit (0);
            case Formula::Until:    
            case Formula::Weak_Until:
            case Formula::Release:
            case Formula::Weak_Release:
            default:
                res += smv_var (f, V, strV);
                break;
        }
        return res;
    }

    /*
    * Main function to create INIT, DEFINE and TRAN statements of the SMV model
    * 
    */
    void Translator::create_smv (Formula *f, f2s& V, string& strV, f2s& Def, string& strDef, f2s& Init, string& strInit, f2s& Invar, string& strInvar, f2s& Tran, string& strTran)
    {
        if (f == NULL)
            return;
        if (Def.find (f) != Def.end ())
            return;
        if (Init.empty ())
        {
            strInit += "\t" + smv_expr (f, V, strV) + "\n";
            Init.insert (std::pair<Formula*, string> (f, strInit));
        }
        string var = "";
        Interval* interval = f->interval ();
        //interval = [a, b]
        if (interval != NULL)
        {
            //special case that [0,0]
            if (interval->_left == 0 && interval->_right == 0) {
                //set f = f->r_mf
                strDef += "\t" + smv_var (f, V, strV) + " := " + smv_expr (f->r_mf (), V, strV) + ";\n";
                Def.insert (std::pair<Formula*, string> (f, ""));
                create_smv (f->r_mf (), V, strV, Def, strDef, Init, strInit, Invar, strInvar, Tran, strTran);
                return;
            }
            
            Formula::opkind prefix_op = Formula::Next;
            string prefix_next = "!Tail & next";
            
            Formula::opkind op = Formula::Next;
            string next =  "!Tail & next" ;
            string imply = sloppy_ ? " -> " : " <-> ";
            
            //special case that interval in f is [0,1]
            if (interval->_left == 0 && interval->_right == 1) {
                //set f = f->r_mf
                strTran += "\t(" + smv_var (f, V, strV) + imply + "(" + prefix_next + "( " + smv_expr (f->r_mf (), V, strV) + "))) & \n";
                Tran.insert (std::pair<Formula*, string> (f, ""));
                create_smv (f->r_mf (), V, strV, Def, strDef, Init, strInit, Invar, strInvar, Tran, strTran);
                return;
            }
            
            Formula *f2 = f->decrease_interval ();
            
            if (mltl0_) {
                if (Invar.find (f) == Invar.end ()) {
                    if (f->oper () == Formula::Until || f->oper () == Formula::Release) {
                        strInvar += "\t(" + smv_var (f2, V, strV) + " -> " + smv_var (f, V, strV) + ") &\n";
                        Invar.insert (std::pair<Formula*, string> (f, ""));
                    }
                    else {
                        strInvar += "\t(" + smv_var (f, V, strV) + " -> " + smv_var (f2, V, strV) + ") &\n";
                        Invar.insert (std::pair<Formula*, string> (f, ""));
                    }
                }
            }
            
            Formula *f3 = Formula (prefix_op, NULL, f2, NULL).unique ();
            //0 < a <= b
            if (interval->_left > 0)
            {
                strDef += "\t" + smv_var (f, V, strV) + " := " + smv_expr (f3, V, strV) + ";\n";
                Def.insert (std::pair<Formula*, string> (f, ""));
                strTran += "\t(" + smv_var (f3, V, strV) + imply + "(" + next + "(" + smv_expr (f2, V, strV) +"))) & \n";
                Tran.insert (std::pair<Formula*, string> (f, ""));
                create_smv (f2, V, strV, Def, strDef, Init, strInit, Invar, strInvar, Tran, strTran);
            }
            else 
            {
                //0 = a < b
                if (interval->_right > 0)
                {
                    string op1 = (f->oper () == Formula::Until || f->oper () == Formula::Weak_Until) ? " | " : " & ";
                    string op2 = (f->oper () == Formula::Until || f->oper () == Formula::Weak_Until) ? " & " : " | ";
                    strDef += "\t" + smv_var (f, V, strV) + " := (" + smv_expr (f->r_mf (), V, strV) + op1 + "(" + smv_expr (f->l_mf (), V, strV) + op2 + smv_var (f3, V, strV) + "));\n";
                    Def.insert (std::pair<Formula*, string> (f, ""));
                    strTran += "\t(" + smv_var (f3, V, strV) + imply + "(" + next + "(" + smv_expr (f2, V, strV) +"))) & \n";
                    Tran.insert (std::pair<Formula*, string> (f3, ""));
                    create_smv (f2, V, strV, Def, strDef, Init, strInit, Invar, strInvar, Tran, strTran);
                    create_smv (f->l_mf (), V, strV, Def, strDef, Init, strInit, Invar, strInvar, Tran, strTran);
                    create_smv (f->r_mf (), V, strV, Def, strDef, Init, strInit, Invar, strInvar, Tran, strTran);
                }
                else //0 = a = b
                {
                    cout << "create_smv error\n";
                    exit (0);
                }
            }
        }
        else
        {
            create_smv (f->l_mf (), V, strV, Def, strDef, Init, strInit, Invar, strInvar, Tran, strTran);
            create_smv (f->r_mf (), V, strV, Def, strDef, Init, strInit, Invar, strInvar, Tran, strTran);
        }
        return;
    }
    
    /*
    * Translation from MLTL formula to SMT-LIB v2 format
    */
    std::string Translator::mltl2smtlib (Formula *f)
    {
        FormulaSet V;
        string res = smtlib_var (f, V);
        std::string start = int2str (0);
        std::string len = "len";
        res += "(define-fun f ((k Int) (len Int)) Bool " + smtlib_expr (f, start, len) +")\n";
        
        //res += "(assert (=(f 0) " + smtlib_expr (f, start) + "))\n";
        res += "(assert (exists ((len Int)) (f 0 len)))\n";
        res += "(check-sat)\n";
        return res;
    }
    
    /*
    * Generate the set of variables for SMT LIB v2 model
    */
    string Translator::smtlib_var (Formula *f, FormulaSet& V)
    {
        string res = "";
        if (f->oper () > Formula::Undefined)
        {
            if (V.find (f) != V.end ())
                res = "";
            else
            {
                res = "(declare-fun " + f->to_string () + " (Int) Bool)\n";
                V.insert (f);
            }
        }
        else
        {
            if (f->l_mf () != NULL)
                res += smtlib_var (f->l_mf (), V);
            if (f->r_mf () != NULL)
                res += smtlib_var (f->r_mf (), V);
        }
        return res;
    }
    
    /*
    * Generate an SMT-LIB v2 expression for the formula 
    * k and len in the result string are fixed 
    */
    string Translator::smtlib_expr (Formula *f, std::string& loc, std::string& len)
    {
        string res = "";
        
        string quantify_var1 = get_quantify_var ();
        string quantify_var2 = get_quantify_var ();
        
        string new_len1 = "(- " + len + " " + quantify_var1 + ")";
        string new_len2 = "(- " + len + " " + quantify_var2 + ")";
        
        switch (f->oper ())
        {
            case Formula::True:
                res += "(and (> " + len + " 0) true)";
                break;
            case Formula::False:
                res += "(and (> " + len + " 0) false)";
                break;
            case Formula::Literal:    
            case Formula::Not:
                res += "(and (> " + len + " 0) (not " + smtlib_expr (f->r_mf (), loc, len) + "))";
                break;
            case Formula::And:
                res += "(and (> " + len + " 0) (and " + smtlib_expr (f->l_mf (), loc, len) + " " + smtlib_expr (f->r_mf (), loc, len) + "))";
                break;
            case Formula::Or:
                res += "(and (> " + len + " 0) (or " + smtlib_expr (f->l_mf (), loc, len) + " " + smtlib_expr (f->r_mf (), loc, len) + "))";
                break;
            case Formula::Undefined:  
                cout << "smtlib_expr error: cannot recognize the MLTL formula\n";
                exit (0);
            case Formula::Weak_Until: //equal to Until
            /*
                res += "(or (<= " + len + " " + int2str (f->interval ()->_left) + ") (exists ((" + quantify_var1 + " Int)) (and (and (>= " + quantify_var1 + " " + "(+ " + loc + " " + int2str (f->interval ()->_left) + ")" + ") (<= " + quantify_var1 + " " + "(+ " + loc + " " + int2str (f->interval ()->_right) + ")" + ")) (and " + "(not " + smtlib_expr (f->r_mf (), quantify_var1, new_len1) + ")" + " (forall ((" + quantify_var2 + " Int)) (or (or (< " + quantify_var2 + " (+ " + loc + " " + int2str (f->interval ()->_left) + ")" + ") (>= " + quantify_var2 + " " + quantify_var1 + ")) " + "(not " + smtlib_expr (f->l_mf (), quantify_var2, new_len2) + ")))))))";
                break;
            */
            case Formula::Until:    
                res += "(and (> "+ len + " " + int2str (f->interval ()->_left) + ") (exists ((" + quantify_var1 + " Int)) (and (and (>= " + quantify_var1 + " " + "(+ " + loc + " " + int2str (f->interval ()->_left) + ")" + ") (<= " + quantify_var1 + " " + "(+ " + loc + " " + int2str (f->interval ()->_right) + ")" + ")) (and " + smtlib_expr (f->r_mf (), quantify_var1, new_len1) + " (forall ((" + quantify_var2 + " Int)) (implies (and (>= " + quantify_var2  + " (+ " + loc + " " + int2str (f->interval ()->_left) + ")" + ") (< " + quantify_var2 + " " + quantify_var1 + ")) " + smtlib_expr (f->l_mf (), quantify_var2, new_len2) + "))))))";
                break;
            case Formula::Weak_Release:// equal to Release
            /*
                res += "(or (<= " + len + " " + int2str (f->interval ()->_left) + ") (forall ((" + quantify_var1 + " Int)) (implies (and (>= " + quantify_var1 + " " + "(+ " + loc + " " + int2str (f->interval ()->_left) + ")" + ") (<= " + quantify_var1 + " " + "(+ " + loc + " " + int2str (f->interval ()->_right) + ")" + ")) (or " + "(not " + smtlib_expr (f->r_mf (), quantify_var1, new_len1) + ")" + " (exists ((" + quantify_var2 + " Int)) (and (and (>= " + quantify_var2 + " (+ " + loc + " " + int2str (f->interval ()->_left) + ")" + ") (< " + quantify_var2 + " " + quantify_var1 + ")) " + "(not " + smtlib_expr (f->l_mf (), quantify_var2, new_len2) + ")))))))";
                break;
            */
            case Formula::Release:
                 res += "(and (> " + len + " " + int2str (f->interval ()->_left) + ") (forall ((" + quantify_var1 + " Int)) (implies (and (>= " + quantify_var1 + " " + "(+ " + loc + " " + int2str (f->interval ()->_left) + ")" + ") (<= " + quantify_var1 + " " + "(+ " + loc + " " + int2str (f->interval ()->_right) + ")" + ")) (or " + smtlib_expr (f->r_mf (), quantify_var1, new_len1) + " (exists ((" + quantify_var2 + " Int)) (and (and (>= " + quantify_var2 + " (+ " + loc + " " + int2str (f->interval ()->_left) + ")" + ") (< " + quantify_var2 + " " + quantify_var1 + ")) " + smtlib_expr (f->l_mf (), quantify_var2, new_len2) + "))))))";
                break;
            
            default: //atoms
                res += "(and (> " + len + " 0) (" + f->to_string () + " " + loc + "))";
                break;
        }
        return res;
    }
    
    
    
 }
