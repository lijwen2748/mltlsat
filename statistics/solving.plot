set datafile separator ","
set autoscale fix
#set ytics rotate by 90
set key inside top left
set xlabel "Number of Formulas"
set ylabel "Accumulated Solving Time (sec)"
set xrange [0:500]
set yrange [0:100000]
set terminal postscript eps color font 'Helvetica,20'
set output "solving.eps"

#set title "Cactus plot for MLTL Satisfiability Checking on Schuppan-collected Benchmarks"
plot "./solving.csv"  every 20 using 1:2 title "LTL-SAT" with linespoints lw 3 ps 2,\
	 "./solving.csv"  every 20 using 1:3 title "LTLf-SAT" with linespoints lw 3 ps 2,\
	 "./solving.csv"  every 20 using 1:4 title "BMC" with linespoints lw 3 ps 2,\
	 "./solving.csv"  every 20 using 1:5 title "KLIVE" with linespoints lw 3 ps 2,\
	 "./solving.csv"  every 20 using 1:6 title "SMT" with linespoints lw 3 ps 2
	 
