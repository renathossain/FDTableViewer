showFDtables: showFDtables.c construct.c display.c
	@gcc -Wall -std=c99 -D_DEFAULT_SOURCE -o showFDtables showFDtables.c construct.c display.c
