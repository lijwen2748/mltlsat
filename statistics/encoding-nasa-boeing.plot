set datafile separator ","
set autoscale fix
#set ytics rotate by 90
set key inside top right
set xlabel "Number of Formulas"
set ylabel "Accumulated Encoding Time (sec)"
set xrange [0:70]
set yrange [0:7500]
set terminal postscript eps color font 'Helvetica,20'
set output "encoding-nasa-boeing.eps"


plot "./encoding-nasa-boeing.csv"  using 1:2 title "SMV" with linespoints lw 3 ps 2,\
	 "./encoding-nasa-boeing.csv"  using 1:3 title "SMT with maximal bound 1000" with linespoints lw 3 ps 2,\
	 "./encoding-nasa-boeing.csv"  using 1:4 title "SMT with maximal bound 10000" with linespoints lw 3 ps 2,\
	 "./encoding-nasa-boeing.csv"  using 1:5 title "SMT with maximal bound 100000" with linespoints lw 3 ps 2
	 #"./encoding.csv"  using 1:5 title "KLIVE" with linespoints lw 3 ps 2,\
	 
