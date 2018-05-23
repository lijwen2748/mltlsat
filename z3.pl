#!/usr/bin/perl

#########
# usage: ./nuxmv.pl N (N is a number between 1 and 9920)
#########

use File::Find;

use POSIX qw/strftime/;
use Time::HiRes qw( time );

my $N = $ARGV[0];

#print strftime('%D %T',localtime);
#my $start = strftime('%D %T',localtime);

#my @benchmarks = ("Benchmarks/random/", "Benchmarks/random0/", "Benchmarks/randomConjunction/");
my $benchmarks = "benchmarks.list";
my @formula_types = ("random", "random0", "randomConjunction");
my $output_dir = "results/";
my $translator = "MLTLConvertor -smtlib";
my $tool = "z3";
my $tool_para = "-smt2";
my $tmpfile = "tmp${N}.z3";


#
if (! (-d "$output_dir") ) {
    mkdir("$output_dir", 0755) or die "Cannot mkdir $output_dir: $!";
} #end if
elsif (! (-w "$output_dir") ) { #check for write permission
    die "ERROR: output directory $output_dir is not writable!";
} #end elseif

#
if (! (-d "${output_dir}${tool}") ) {
    mkdir("${output_dir}${tool}", 0755) or die "Cannot mkdir ${output_dir}${tool}: $!";
} #end if
elsif (! (-w "${output_dir}${tool}") ) { #check for write permission
    die "ERROR: output directory ${output_dir}${tool} is not writable!";
} #end elseif

for (my $i = 0; $i < @formula_types; $i ++) {
    #
    my $type = $formula_types[$i];
    if (! (-d "${output_dir}${tool}/${type}") ) {
        mkdir("${output_dir}${tool}/${type}", 0755) or die "Cannot mkdir ${output_dir}${tool}/${type}!";
    } #end if
    elsif (! (-w "${output_dir}${tool}/${type}") ) { #check for write permission
        die "ERROR: output directory ${output_dir}${tool}/${type} is not writable!";
    } #end elseif
}

my $timeout = 300;
my $preprocess_time = 0; #default

##set the input file
open (BENCHMARK, "<${benchmarks}") or die "cannot open the file ${benchmarks}!\n";
my $counter = 1;
my $input_file = "";
while (my $line = <BENCHMARK>) {
    if ($counter == $N) {
        chomp ($line);
        $input_file = $line;
        last;
    }
    $counter ++;
}
###


#####start to run tools with the input
open(INFILE, "<$input_file") or die "cannot open the input file!";
my $output_file = $input_file;
$output_file =~ s/Benchmarks/${tool}/g;
open(OUTFILE, ">${output_dir}${output_file}") or die "cannot open the file!";
                
print "Checking ${input_file}\n";
my $line = <INFILE>;
      
while ($line ne "") {
    eval {
            local $SIG{ALRM} = sub { die "timeout\n" };
	        alarm($timeout);
            my $preprocess_begin = time ();
            my $preprocess = `${translator} "$line"`;
            my $preprocess_end = time ();
            $preprocess_time = (int(($preprocess_end - $preprocess_begin) * 1000))/1000;
            
            open (TMP, ">$tmpfile") or die "cannot write to the temp file!\n";
            print TMP "$preprocess\n";
            close (TMP);
                                            
            my $run_begin = time ();
            $output = `$tool ${tool_para} "${tmpfile}"`;
	        chomp ($output);
	        if ($output =~ /unsat/) {
	            $output = "unsat";
	        } elsif ($output =~ /sat/) {
	            $output = "sat";
	        } else {
	            $output = "unknow";
	        }
	                        
	        my $run_end = time ();
	        my $runtime = (int(($run_end - $run_begin) * 1000))/1000;
	        print OUTFILE "$output\t\t$preprocess_time\t\t$runtime\n";
	   
	        alarm(0);
	 };
	 if($@) {
            $output = `killall -9 $tool`;
            print OUTFILE "timeout\t\t$preprocess_time\t\t$timeout\n";
	        sleep 1;		 
	 }
	 $line = <INFILE>;
}
close(INFILE);
close(OUTFILE);

if (-e "$tmpfile") {
    `rm $tmpfile`;
}

exit;
####


