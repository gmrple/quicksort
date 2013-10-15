.PHONY: all clean
warn = -Wall -Wextra -pedantic
optim = -O0 -g
opt = $(warn) $(optim) 

all : mmap

mmap : quicksort.c
	cc quicksort.c -o mmap $(opt) 

clean :
	rm mmap