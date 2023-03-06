CC = gcc
CCFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE

%.o: %.c design.h
	@$(CC) $(CCFLAGS) -c $<

showFDtables: showFDtables.o construct.o display.o
	@$(CC) $(CCFLAGS) -o showFDtables showFDtables.o construct.o display.o

.PHONY: clean
clean:
	@rm *.o