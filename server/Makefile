CC = gcc
CFLAGS = -Wall -g
CROSS_COMPILER_PREFIX= aarch64-none-linux-gnu-
ifeq ($(CROSS_COMPILE),)
	# NATIVE COMPILER
	COMPILER=$(CC)
else
	#CROSS_COMPILER
	COMPILER=$(CROSS_COMPILER_PREFIX)$(CC)		
endif

default : 
	$(CC) $(CFLAGS) -o aesdsocket aesdsocket.c

all : 
	$(CC) $(CFLAGS) -o aesdsocket aesdsocket.c


clean :
	rm -f aesdoscket aesdsocket.o
	
run :
	./aesdsocket

run_daemon :
	./aesdsocket -d
