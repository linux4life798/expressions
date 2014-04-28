CFLAGS += -Wall -O0
LDFLAGS += -Wall -O0

CFLAGS += -ggdb
LDFLAGS += -ggdb

CFLAGS += -DDEBUG # Enable debugging stuff
#CFLAGS += -DNDEBUG # Old way to disabe assert


.PHONY: all clean docs docsquiet

all: expr docsquiet

expression.o: expression.h expression.c
types.o: types.h types.c
errors.o: errors.h errors.c

expr: errors.o types.o expression.o main.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $+

docs:
	doxygen Doxyfile

docsquiet:
	doxygen Doxyfile > doxygen.log

clean:
	$(RM) *.o expr
	$(RM) -r docs/*
	$(RM) doxygen.log
