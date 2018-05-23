set datafile separator ","
set autoscale fix
#set ytics rotate by 90
set key inside top left
set xlabel "Solving Time without Encoding Heuristics (sec)"
set ylabel "Solving Time with Encoding Heuristics (sec)"
set xrange [0:10]
set yrange [0:10]
set terminal postscript eps color font 'Helvetica,20'
set output "solving0.eps"

#set title "Cactus plot for MLTL Satisfiability Checking on Schuppan-collected Benchmarks"
plot "./solving0.csv"   using 2:4 title "BMC" with points lw 3 ps 2,\
     "./solving0.csv"   using 3:5 title "KLIVE" with points lw 3 ps 2,\
	 #"./solving.csv"   using 1:3 title "LTLf-SAT" with linespoints lw 3 ps 2,\
	 #"./solving.csv"   using 1:4 title "BMC" with linespoints lw 3 ps 2,\
	 #"./solving.csv"   using 1:5 title "KLIVE" with linespoints lw 3 ps 2,\
	 #"./solving.csv"   using 1:6 title "SMT" with linespoints lw 3 ps 2
	 
