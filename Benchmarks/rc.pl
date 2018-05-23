#!/usr/bin/perl
#
# by: Kristin Yvonne Rozier
# begun: October/23/2017

# Generate a random conjunction of $f MTL formulas over $v variables for a timeline of steps <= $t (i.e., $t is the maximum integer time).

#Declare the MTL formulas
# @MTL_Formulas = qw(
# Absence2
# AlternativePrecedence
# AlternativeResponse
# AlternativeSuccession
# ChainPrecedence
# ChainResponse
# ChainSuccession
# Choice
# Coexistence
# End
# ExclusiveChoice
# Existence
# NegativeChainSuccession
# NegativeSuccession
# NotCoexistence
# Precedence
# RespondedExistence
# Response
# Succession
# );
#NOTE: all of these use a and b as the variables

#Declare the MTL formulas
@MTL_Formulas = glob("MTL_patterns/*");

$debug = 0;

if ($debug) {
    print "Choosing from these patterns: @MTL_Formulas\n";
} # end if


#################### Argument Setup ####################

#Check for correct number and type of command line arguments

if (($ARGV[0] =~ /--help/) || (@ARGV != 3)) {
    die "Usage: MTLRandomConjunction.pl NumVariables NumConjuncts MaxTime\nRandomly generate an MTL formula containing NumConjuncts ANDed together. The variables in each formula will be chosen randomly from NumVars variables. The time ranges for each temporal operator will be randomly set according to realistic patterns, with all times less than MaxTime (so essentially MaxTime is both the maximum mission length and the maximum range of an operator time interval)\n";
} #end if

$v = $ARGV[0]; #NumVariables
$conj = $ARGV[1]; #NumConjuncts
$t = $ARGV[2]; #MaxTime

#Check for errors
if ( (! ($v =~ /^\d+$/)) || (! ($conj =~ /^\d+$/)) || (! ($t =~ /^\d+$/)) ) {
    print STDERR "ERROR: NumVariables and NumConjuncts and MaxTime must all be positive integers.\n";
    die "Usage: MTLRandomConjunction.pl NumVariables NumConjuncts MaxTime\nRandomly generate an MTL formula containing NumConjuncts ANDed together. The variables in each formula will be chosen randomly from NumVars variables. The time ranges for each temporal operator will be randomly set according to realistic patterns, with all times less than MaxTime (so essentially MaxTime is both the maximum mission length and the maximum range of an operator time interval)\n";
} #end if
else { #debug
    if ($debug) {
	print STDERR "Generating $c conjuncts over $v variables with max time $t\n";
    } #end if
} #end else


##################################################################
#
# Main Program: 
# 
#    Generate $c conjuncts; for each one, choose an MTL formula
#       from the list, assign its variables to one of $v in
#       the set, and assign its temporal operators ranges in 0...$t. 
#
# Assumptions:
#
#    This loop relies on the fact that there is a directory called
#       MTL_patterns with each pattern in a single file. In each 
#       pattern, the following hold:
#       - variables are named p1, p2, p3, ...
#       - there are never more than three different variables 
#         in one pattern (yet, otherwise update the code)
#       - time ranges are all of the form [t_min,t_max]
#
# Given the assumptions hold, the loop finds and replaces variables
#       and times then conjuncts the resulting formulas together.
##################################################################

for ($i = 0; $i < $conj; $i++) {
    
    #Pick an LTLf formula to add to the conjunction
    $NumFormulas = @MTL_Formulas; 
    $f_num = int(rand($NumFormulas)); 
    if ($debug) {
	print STDERR "Choosing formula number $f_num from the set of $NumFormulas formulas, which is formula $MTL_Formulas[$f_num]\n";
    } #end if    

    $conjunct = $MTL_Formulas[$f_num];

#TODO: modify this next part to:
# 1) assign time bounds realistically but otherwise randomly
# 2) make an efficient loop to replace both variables and times

    
    $f = `cat $conjunct`;  #get formula
    chomp $f;              #remove trailing \n
    $v_num = int(rand($v)); #get the number of the first variable
    $a = "p$v_num";
    $v_num = int(rand($v)); #get the number of the second variable
    $b = "p$v_num";
    $v_num = int(rand($v)); #get the number of the third variable
    $c = "p$v_num";
    
    $f =~ s/ a/ $a/g;
    $f =~ s/a /$a /g;
    $f =~ s/~a/~$a/g;
    $f =~ s/ b/ $b/g;
    $f =~ s/b /$b /g;
    $f =~ s/~b/~$b/g;
    $f =~ s/ c/ $c/g;
    $f =~ s/c /$c /g;
    $f =~ s/~c/~$c/g;
    
    #generate intervals
    while ($f =~ /t_min, t_max/)
    {
        my $min = int (rand($t));
        my $max = $min + int (rand($t-$min));
        $f =~ s/t_min, t_max/$min, $max/g;
    }

    
    if ($i > 0) {
	$formula = "$formula & ($f)";
    } #end if    
    else {
	$formula = "($f)";
    } #end else

} #end for

print "$formula\n";













