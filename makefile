.PHONY: all clean
warn = -Wall -Wextra -pedantic
optim = -O0 -g
opt = $(warn) $(optim) 

all : mmap

quicksort : quicksort.c
	cc quicksort.c -o quicksort $(opt) 

clean :
	rm mmap
