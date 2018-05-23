# Parser Makefile
# need flex bison

PARSERCFILES =	mltllexer.c mltlparser.c
PARSERHFILES =	mltllexer.h mltlparser.h
ALLCFILES   =	$(PARSERCFILES) mltl_formula.c trans.c main.c
CC	    =   g++
CDEBUG	    =	-g

parser:	$(PARSERCFILES)

mltllexer.c:	mltllexer.l
		flex mltllexer.l

mltlparser.c:	mltlparser.y
		bison mltlparser.y

run:	$(ALLCFILES)
	$(CC) $(ALLCFILES) -o parser

run_debug:  $(ALLCFILES)
	    $(CC) $(CDEBUG) $(ALLCFILES) -o parser

run_release:	run

clean:
	rm -f *.o *~ $(PARSERHFILES) $(PARSERCFILES) parser

