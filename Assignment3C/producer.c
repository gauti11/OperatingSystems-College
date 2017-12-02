#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAXLEN 100

int main(int argc, char *argv[])
{
	int fd;			//File descriptor
	char numstr[MAXLEN];
	int count = 0;
	int num_to_write;
	int return_value;
	ssize_t ret;

	fd = open("/dev/numpipe", O_WRONLY);		//Open character device in write mode only

	while(1)
	{
		bzero(numstr, MAXLEN);			//Add zero bytes into numstr variable
		sprintf(numstr, "%d%d\n", getpid(), count++);	
		num_to_write = atoi(numstr);
		printf("Writing: %d", num_to_write);
		ret = write(fd, &num_to_write, sizeof(int));	//Send data to character device
		if ( ret < 0) 
		{
			fprintf(stderr, "error writing ret=%ld errno=%d perror: ", ret, errno);
			perror("");
		}
		else
		{
			printf("\nBytes written: %ld\n", ret);
		}
		sleep(1);
	}

	close(fd);
	return 0;
}
