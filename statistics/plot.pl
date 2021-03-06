#!/usr/bin/perl

#########
# usage: ./plot.pl
#########

my $root = "..";
my $formula_list = "${root}/benchmarks.list";
my $output_dir = "${root}/results/";
my @tools = ("aalta", "aaltaf", "nuXmv-bmc", "nuXmv-klive", "z3");
my $output_encoding = "./encoding.csv";
my $output_solving = "./solving.csv";
my @encoding = (0, 0, 0, 0, 0);
my @solving = (0, 0, 0, 0, 0);
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
    my @results = ("unknow", "unknow", "unknow", "unknow", "unknow");
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
            
            if ($data[0] ne "unknow" && $data[0] ne "timeout") {
                $results[$i] = $data[0];
            }
            
            $encoding_cost = $encoding[$i] + $data[1];
            $solving_cost = $solving[$i] + $data[2];    
        }
        
        
        
        $encoding[$i] = $encoding_cost;
        $solving[$i] = $solving_cost;
            
        $result_encoding[$i] = $result_encoding[$i] + ${encoding_cost};
        $result_solving[$i] = $result_solving[$i] + ${solving_cost};
        close (IN);
    }
    
    ###checking correctness
        my $result = "unknow";
        for (my $i = 0; $i < @results; $i ++) {
            if ($results[$i] ne "unknow") {
                if ($result eq "unknow") {
                    $result = $results[$i];
                }
                elsif ($result ne $results[$i]) {
                    print ("Error found at ${line}:\n$tools[0] ($results[0])\n$tools[1] ($results[1])\n$tools[2] ($results[2])\n$tools[3] ($results[3])\n$tools[4] ($results[4])\n");
                    last;
                }
            }
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
