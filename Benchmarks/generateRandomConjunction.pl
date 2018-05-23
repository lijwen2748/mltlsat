#!/usr/bin/perl

use FileHandle;      #for open() 

#Set the directory where all of the formulas will be stored
$formula_dir = "./randomConjunction";

#If the formula directory doesn't exist, create it
if (! (-d $formula_dir) ) {
    mkdir("$formula_dir", 0755) or die "Cannot mkdir $formula_dir: $!";
} #end if
elsif (! (-w $formula_dir) ) { #check for write permission
    die "ERROR: formula directory $formula_dir is not writable!";
} #end elseif

my @VList = (5, 10);             #list of maximal variable numbers
my @CList = (5, 10, 15, 20);       #list of Conjuct numbers
my @BList = (50, 100, 500);         # list of maximal bound values
my $N = 20;      #number of generated formulas for each category. 

#################### Generate the sets of formulas ####################

for (my $i = 0; $i < @VList; $i ++) { #for n variables
    my $v_num = $VList[$i];

    for (my $j = 0; $j < @CList; $j ++) { #for each conjunct
	my $conj_num = $CList[$j];

	for (my $k = 0; $k < @BList; $k ++) { #for each bound
	    my $b_num = $BList[$k];
	    
	    print "\n\nV = $v_num, C = $conj_num, B = $b_num:\n";
	     

	    for (my $n = 1; $n <= $N; $n ++) { #generate $N formulas
		
		#Create an output file for each set of formulas data
	    $FormFile = "${formula_dir}/V${v_num}C${conj_num}B${b_num}-${n}.mltl";
	    open(FORMULAS, ">$FormFile") or die "Could not open $FormFile";
	    
		$formula = `./rc.pl ${v_num} ${conj_num} ${b_num}`;
		print FORMULAS "$formula";
		
		close(FORMULAS) or die "Could not close $FormFile";
	    } #end for each formula

	        
	} #end for each bound

    } #end for each conjunct

} #end for n variables


