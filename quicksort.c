#include "stdio.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "errno.h"

#define QS_MIN_SZ 100

int map_file(int fd, unsigned char rw, void** mapped_addr, size_t size);
int unmap_file(void* map, size_t size);
void quicksort(int * arr, int size);
void insertion_sort(int* arr, int size);
extern int errno;

int main(){
    int fd[2] = {-1,-1};
    const char* fname = "rand.bin";
    const char* out_fname = "sorted.bin";
    size_t size;
	int status = -1;
    struct stat s;
    int* infile_mapped = NULL;
    int* outfile_mapped = NULL;
	int i =0;
	int t = 0;

    /* open the file for reading */
    fd[0] = open(fname, O_RDONLY);

    /* validate that the file descriptor
        is in a good range */
    if (fd[0] < 0){
        printf("invalid file\n");
        return -1;
    }

    /* get the file's size */

    /* request the file's metadata
        from the os */
    status = fstat(fd[0],&s);

    if (status){
        printf("fstat failed\n");
    }

    /* grab the filesize from the 
        metadata struct */

    size = s.st_size;

    if (size == 0){
        printf("empty file, exiting\n");
        return -1;
    }
   
   	printf("in file map\n");

    status = map_file(fd[0], 1, (void**)&infile_mapped, size);

    if (status)
        return -1;

	/* now lets create an output file and map it too */
	
	/* 
	*  note that if you want the permissions to be set
	*  exactly like this, you probably need to have 
	*  umask changed, by default on ubuntu it should be
	*  0002, which will make the effective permissions
	*  0664, which will not prohibit this from running,
	*  but it is a good thing to keep in mind when using
	*  the open syscall
	*/
	fd[1] = open(out_fname,O_CREAT|O_TRUNC|O_RDWR, 0666);
  
   	if (fd[1] < 0){
		printf("out file open faild\n");
		return -1;
	}

  	/* go to the end of the file */	
	if (lseek (fd[1], size - 1, SEEK_SET) == -1){
		printf("Error, could not get to last byte\n");
	}

	/* try to write a dummy byte at end so file is correct size */	
	if (write (fd[1], "", 1) != 1){
		printf("failed to write dummy byte\n");
		return -1;
	}

	printf("outfile map\n");
    status = map_file(fd[1], 0, (void**)&outfile_mapped, size);
  
  	if (status)
		return -1;

	/* for each int (4 bytes) */
	for (i=0; i<((int)size>>2); i++)
	{
		t = infile_mapped[i];		
		outfile_mapped[i] = t;
	}

    /* we don't really care about the status
        because we're quitting directly after */ 
    unmap_file( infile_mapped, size);
	close(fd[0]);

	quicksort(outfile_mapped, 50);/*size >> 2);*/

	for (i=0; i<50; i++){
		printf("%d\n",outfile_mapped[i]);
	}
	unmap_file(outfile_mapped, size);
	close(fd[1]);

    return 0;
}

inline void swap( int* a, int* b){
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

/***********************************
* Quicksort
* A quick in place implementation
* of quicksort. This will pick the 
* best of 3 values for the pivot
***********************************/
void quicksort(int* arr, int size){
	if (size < QS_MIN_SZ){
		insertion_sort(arr, size);
	}
}

void insert(int* arr, int val, int idx, int end_idx){
	/* start at the index that the value goes */
	int i = idx;
	int temp = arr[idx];	
	int temp1 = 0;

	/* shift everything starting at that index over 1 */
	for (; i<end_idx; i++){
		temp1 = arr[i+1];	
		arr[i+1] = temp;
		swap(&temp, &temp1);
	}

	/* now that the shifting is done, put the value at the correct index */
	arr[idx] = val;
}

void insertion_sort(int* arr, int size){
	int i = 1;
	int j = 0;

	for (; i<size; i++){
		/* find each element's place */
		for (j = 0; j < i; j++){
			if (arr[i] < arr[j]){
				insert(arr, arr[i], j, i);
				break;
			}
		}
	}
}
	

/*  
*   rw == 1 :  read
*   rw == 0 :  write
*/

int map_file(int fd, unsigned char rw, void** mapped_addr, size_t size){
    int rw_flags = 0;
    void* map = NULL;
    int err = 0;
	void** ret = mapped_addr;
	int map_flag = 0;	
    
    /* determine rw flags */
    if (rw){
        rw_flags |= PROT_READ;
		map_flag = MAP_SHARED;
    } else {
        rw_flags |= PROT_WRITE; 
		map_flag = MAP_SHARED;
    }

    map = mmap(NULL, size, rw_flags, map_flag, fd, 0);

    if (map == MAP_FAILED){
		err = errno;	
        printf("mapping failed: %d\n", err);
        return -1;
    }

	*ret = map;
    return 0;
}

int unmap_file(void* map, size_t size){
    int status =munmap(map, size);

    if (status){
        printf("unmap failed!\n");
    }

    return status;
}

