#include<fcntl.h>
#include<stdio.h>
#include<sys/mman.h>
#include<unistd.h>
#include<stdlib.h>
#include<x86intrin.h>

#define PAGE_SIZE 4096
#define READ_SIZE 4096
#define DELAY 2000000000

int main(int argc, char** argv){
	int fd, i, j, amount_read;
	unsigned long long t;
	void *mapaddr;
	volatile int throw_away;
	char buffer[READ_SIZE];
	unsigned char c;

        if(argc!=3){
                printf("Usage: %s shared_file data_file\n", argv[0]);
                return 1;
        }

        // Setup fd of shared_file and mmap
        fd = open(argv[1], O_RDONLY);
	
	// mmap
	mapaddr = mmap(NULL,PAGE_SIZE,PROT_READ,MAP_SHARED,fd,0);
	close(fd);

	// Open data_file
	fd = open(argv[2], O_RDONLY);

	// Read until we read zero
	while( (amount_read = read(fd, buffer, READ_SIZE)) ){
		// Run through amount_read bytes
		for(i = 0; i<amount_read; i++){
			// Get byte
			c = buffer[i];
			// Transmit bit by bit
			for(j = 0; j<8; j++){
				t = __rdtsc(); 
				do{
					if(c & 0x80){
						throw_away = *((char*)mapaddr);
					}
				}while((__rdtsc()-t)<DELAY);
				c = c << 1;
			}
		}
	}
}
