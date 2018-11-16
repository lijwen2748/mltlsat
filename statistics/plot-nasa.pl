#!/usr/bin/perl

#########
# usage: ./plot-nasa.pl
#########

my $root = "..";
my $formula_list = "${root}/nasa-boeing.list";
my $data_dir = "${root}/results/nasa-boeing/";
my @tools = ("nuXmv-bmc", "z3-1000", "z3-10000", "z3-100000");
my $output_encoding = "./encoding-nasa-boeing.csv";
my $output_solving = "./solving-nasa-boeing.csv";
my @encoding = (0, 0, 0, 0);
my @solving = (0, 0, 0, 0);
my $timeout = 3600;

open (ENCODING_OUT, ">${output_encoding}") or die "cannot open ${output_encoding}\n";
open (SOLVING_OUT, ">${output_solving}") or die "cannot open ${output_solving}\n";

open (FORMULA, "<${formula_list}") or die "cannot open ${formula_list}!\n";
my $formula_start = 1;
my $formula_end = 500;
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
    my @result_encoding = (0, 0, 0, 0, 0);
    my @result_solving = (0, 0, 0, 0, 0);
    for (my $i = 0; $i < @tools; ++ $i) {
        my $line_cp = $line;
        my $tool = $tools[$i];
        $line_cp = $data_dir.$tool."/".$line_cp;
        open (IN, "<${line_cp}") or die "cannot open file ${line_cp}!\n";
        my $line_res = <IN>;
        chomp ($line_res);
        my $encoding_cost = 0;
        my $solving_cost = 0;
        if ($line_res eq "") {
            $encoding[$i] = $encoding[$i] + $timeout;
            $solving[$i] = $solving[$i] + $timeout;
            $encoding_cost = $encoding[$i];
            $solving_cost = $solving[$i];  
        }
        else {
            my @data = split ("\t\t", $line_res);
            if (@data != 3) {
                die "extract the result error: ${line}!\n";
            }       
            
            $encoding_cost = $encoding[$i] + $data[1];
            if ($data[0] eq "unknow") {
            	$solving_cost = $solving[$i] + $timeout;
            }
            else {
            	$solving_cost = $solving[$i] + $data[2];   
            } 
        }
        
        
        $encoding[$i] = $encoding_cost;
        $solving[$i] = $solving_cost;
            
        $result_encoding[$i] = $result_encoding[$i] + ${encoding_cost};
        $result_solving[$i] = $result_solving[$i] + ${solving_cost};
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
