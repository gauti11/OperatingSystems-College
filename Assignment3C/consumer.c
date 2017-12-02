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

	int fd;
	long int bytes_read;
	char numstr[MAXLEN];
	int num_read;
	fd = open("/dev/numpipe", O_RDONLY);
	if(fd < 0)
        {
                perror("");
                printf("error opening character device");
                exit(1);
        }
	while (1)
	{
		bytes_read = read(fd, &num_read, sizeof(int));
		
		if(bytes_read < 0)
		{
			fprintf(stderr, "error reading ret=%ld errno=%d perror: ", bytes_read, errno);
                        perror("");
		}
		else
		{
			printf("\nNumber read: %d", num_read);
			printf("\nData consumed by consumer: %ld", bytes_read);
		}
		sleep(1);
	}
	close(fd);
	return 0;
}


