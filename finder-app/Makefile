CC = gcc
CFLAGS = -Wall
CROSS_COMPILER_PREFIX= aarch64-none-linux-gnu-

ifeq ($(CROSS_COMPILE),)
	# NATIVE COMPILER
	COMPILER=$(CC)
else
	#CROSS_COMPILER
	COMPILER=$(CROSS_COMPILER_PREFIX)$(CC)		
endif

default :
	$(COMPILER) $(CFLAGS) writer.c -o writer

clean :
	rm writer

run :
	./writer