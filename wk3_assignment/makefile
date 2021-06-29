#basic from class
#CC=arm-linux-gnueabi-gcc
CC=gcc
CCFLAGS=
INCLUDES=
LFLAGS=-L/usr/lib/x86_64-linux-gnu
LIBS=-lcurl -lpthread

SRC=communication.c
OBJ=$(SRC:.c=.o)
MAIN=input_test

RM=rm - rf

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(MAIN): $(OBJ)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LFLAG) $(LIBS)

all: $(MAIN)

clean:
	$(RM) $(MAIN) *.o *~
