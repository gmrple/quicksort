warn = -Wall -Wextra -pedantic
optim = -O0 -g
opt = $(warn) $(optim) 

cmap : quicksort.c
	cc quicksort.c -o mmap $(opt) 
