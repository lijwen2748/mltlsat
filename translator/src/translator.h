/* 
 * File:   translator.h
 * Author: Jianwen Li
 *
 * Created on December 26, 2017
 */

#ifndef TRANSLATOR_H
#define	TRANSLATOR_H

#include "formula.h"
#include "util/utility.h"
#include <string>

namespace mltl 
{
    typedef hash_map<Formula*, std::string, Formula::mf_prt_hash> f2s;
    typedef hash_set<Formula*, Formula::mf_prt_hash> FormulaSet;
    
    class Translator 
    {
    public:
        //constructor
        Translator (Formula *f, bool mltl0) : f_(f), sloppy_ (false), mltl0_ (mltl0) {}
        ~Translator () {}
        
        //translator functions
        inline std::string mltl2ltlf () {return mltl2ltl (f_, true);}
        inline std::string mltl2ltl () {return (mltl2ltl (f_, false) + "& (F Tail)");}
        inline std::string mltl2smv () {return mltl2smv (f_);}
        inline std::string mltl2smtlib () {return mltl2smtlib (f_);}
        
    private:
        Formula *f_;
        static int id_; //counter to create variables
        
        //flags
        bool sloppy_;
        bool mltl0_;  //whether invoke heuristics for MLTL0
        
        //helper functions
        std::string mltl2ltl (Formula *f, bool finite);
        std::string mltl2smv (Formula *f);
        
        
        std::string smv_var (Formula *f, f2s& V, std::string& strV);
        std::string smv_expr (Formula *f, f2s& V, std::string& strV);
        void create_smv (Formula *f, f2s& V, std::string& strV, f2s& Def, std::string& strDef, f2s& Init, std::string& strInit, f2s& Invar, std::string& strInvar, f2s& Tran, std::string& strTran);
        std::string mltl2smtlib (Formula *f);    
        std::string smtlib_var (Formula *f, FormulaSet& V);
        std::string smtlib_expr (Formula *f, std::string& loc, std::string& len);
        inline std::string get_quantify_var ()
        {
            std::string res = "v";
            return res + int2str (id_++);
        }
    };
}

#endif
