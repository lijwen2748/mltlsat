#include <iostream>
#include <assert.h>
#include "translator.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>

using namespace std;
using namespace mltl;

bool to_ltlf = false;
bool to_ltl = false;
bool to_smv = false;
bool to_smt = false;
bool mltl0 = false;
string input = "";


void print_helper ()
{
    cout << "MLTLConvertor <-ltlf | -ltl | -smv | -smv0 | -smtlib> FORMULA\n";
    cout << "       -ltlf         :       convert MLTL to LTLf\n";
    cout << "       -ltl          :       convert MLTL to LTL\n";
    cout << "       -smv          :       convert MLTL to SMV\n";
    cout << "       -smv0         :       convert MLTL0 to SMV (Heuristic enabled)\n";
    cout << "       -smtlib       :       convert MLTL to SMT-LIB v2.0\n";
}

void parse_args (int argc, char *argv[]) 
{
    if (argc != 3)
    {
        print_helper ();
        exit (0);
    }
    
    if (strcmp (argv[1], "-ltlf") == 0)
        to_ltlf = true;
    else if (strcmp (argv[1], "-ltl") == 0)
        to_ltl = true;
    else if (strcmp (argv[1], "-smv") == 0)
        to_smv = true;
    else if (strcmp (argv[1], "-smv0") == 0) {
        to_smv = true;
        mltl0 = true;
    }
    else if (strcmp (argv[1], "-smtlib") == 0)
        to_smt = true;
    else 
    {
        print_helper ();
        exit (0);
    }
    
    input = string (argv[2]);
}

int main (int argc, char *argv[]) 
{
    parse_args (argc, argv);
    Formula *f = Formula (input.c_str ()).unique ();
    assert (f != NULL);
    Translator t (f, mltl0);
    if (to_ltlf)
        cout << t.mltl2ltlf () << endl;
    else if (to_ltl)
        cout << t.mltl2ltl () << endl;
    else if (to_smv)
        cout << t.mltl2smv () << endl;
    else if (to_smt)
        cout << t.mltl2smtlib () << endl;
    else
        cout << "No output format is chosen!\n";
    return 0;
}
