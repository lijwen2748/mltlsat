set datafile separator ","
set autoscale fix
#set ytics rotate by 90
set key inside top center
set xlabel "Number of Formulas"
set ylabel "Accumulated Total Time (min)"
set xrange [0:65]
set yrange [0:300]
set terminal postscript eps color font 'Helvetica,20'
set output "total-nasa-boeing.eps"

plot "./total-nasa-boeing.csv"  every 20 using 1:2 title "BMC-1000" with linespoints lw 3 ps 2,\
     "./total-nasa-boeing.csv"  every 20 using 1:3 title "KLIVE-1000" with linespoints lw 3 ps 2,\
	 "./total-nasa-boeing.csv"  every 20 using 1:4 title "Z3-1000" with linespoints lw 3 ps 2,\
	 "./total-nasa-boeing.csv"  every 20 using 1:5 title "Z3-10000" with linespoints lw 3 ps 2,\
	 "./total-nasa-boeing.csv"  every 20 using 1:6 title "Z3-100000" with linespoints lw 3 ps 2
	 
