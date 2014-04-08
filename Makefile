#CFLAGS += -Wall
#LDFLAGS += -Wall

CFLAGS += -ggdb
LDFLAGS += -ggdb

CFLAGS += -DDEBUG # Enable debugging stuff
#CFLAGS += -DNDEBUG # Old way to disabe assert


.PHONY: all clean

all: expr

expr: errors.o expression.o main.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $+

clean:
	$(RM) *.o expr
