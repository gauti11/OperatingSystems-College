#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXLEN 100

int main(int argc, char *argv[])
{
	int fd;				//File descriptor
	long int bytes_read;
	char numstr[MAXLEN];		
	int num_read;
	fd = open("/dev/numpipe", O_RDONLY);		//Open character device in read mode only
	if(fd < 0)		//If characted device file couldn't be opened
        {
                perror("");
                printf("error opening character device");
                exit(1);
        }
	while (1)
	{
		bytes_read = read(fd, &num_read, sizeof(int));		//Read from character device
		
		if(bytes_read < 0)		//If unable to read from character device
		{
			fprintf(stderr, "error reading ret=%ld errno=%d perror: ", bytes_read, errno);
                        perror("");
		}
		else		//If successful read from character device
		{
			printf("\nNumber read: %d", num_read);
			printf("\nData consumed by consumer: %ld", bytes_read);
		}
		sleep(1);
	}
	close(fd);		//Close character device
	return 0;
}


