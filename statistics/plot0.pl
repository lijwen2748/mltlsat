#!/usr/bin/perl

#########
# usage: ./plot0.pl
#########

my $root = "..";
my $formula_list = "${root}/benchmarks.list";
my $output_dir = "${root}/results/";
my @tools = ("nuXmv-bmc", "nuXmv-klive", "nuXmv-bmc0", "nuXmv-klive0");
my $output_encoding = "./encoding0.csv";
my $output_solving = "./solving0.csv";


open (ENCODING_OUT, ">${output_encoding}") or die "cannot open ${output_encoding}\n";
open (SOLVING_OUT, ">${output_solving}") or die "cannot open ${output_solving}\n";

open (FORMULA, "<${formula_list}") or die "cannot open ${formula_list}!\n";
my $formula_start = 501;
my $formula_end = 1000;
my $counter = 0;
while (my $line = <FORMULA>) {
    $counter = $counter + 1;
    if ($counter < $formula_start) {
        next;
    }
    if ($counter > $formula_end) {
        last;
    }

    chomp ($line);
    my @result_encoding = (0, 0, 0, 0);
    my @result_solving = (0, 0, 0, 0);
    for (my $i = 0; $i < @tools; $i ++) {
        my $line_cp = $line;
        my $tool = $tools[$i];
        $line_cp =~ s/Benchmarks/$tool/g;
        $line_cp = $output_dir.$line_cp;
        open (IN, "<${line_cp}") or die "cannot open file ${line_cp}!\n";
        my $line_res = <IN>;
        chomp ($line_res);
        my $encoding_cost = 0;
        my $solving_cost = 0;
        if ($line_res eq "") {
            $encoding_cost = 300;
            $solving_cost = 300;  
        }
        else {
            my @data = split ("\t\t", $line_res);
            if (@data != 3) {
                die "extract the result error: ${line}!\n";
            }       
            $encoding_cost = $data[1];
            $solving_cost = $data[2];    
        }
        
            
        $result_encoding[$i] = ${encoding_cost};
        $result_solving[$i] = ${solving_cost};
        close (IN);
    }
    
    my $index = $counter - $formula_start + 1;
    print ENCODING_OUT "$index,";
    print SOLVING_OUT "$index,";
    for (my $i = 0; $i < @tools; $i ++) {
        
        my $data = $result_encoding[$i];
        print ENCODING_OUT "$data,";
        
        $data = $result_solving[$i];
        print SOLVING_OUT "$data,";
    }
    print ENCODING_OUT "\n";
    print SOLVING_OUT "\n";
    
}
close (ENCODING_OUT);
close (SOLVING_OUT);
close (FORMULA);
exit;
