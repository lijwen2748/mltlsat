# Parser Makefile
# need flex bison

SOURCEFILES     =   parser/mltllexer.c parser/mltlparser.c parser/mltl_formula.c parser/trans.c util/utility.cpp formula.cpp translator.cpp main.cpp
CC	    =   g++
CDEBUG	    =	-g
INCLUDE     =   -I./ -I./parser/


parser/mltlexer.c:  parser/mltlexer.l   
	flex parser/mltlexer.l

parser/mltlparser.c:    parser/mltlparser.y
	bison parser/mltlparser.y

run:	$(SOURCEFILES)
	$(CC) $(INCLUDE) $(SOURCEFILES) -o MLTLConvertor

debug:  $(SOURCEFILES)
	$(CC) $(INCLUDE) $(CDEBUG) $(SOURCEFILES) -o MLTLConvertor

release:    run



clean:
	rm -f *.o *~ MLTLConvertor

