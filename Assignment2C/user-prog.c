#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<linux/kernel.h>
#include<unistd.h>
#include<sys/time.h>

#define N 5
#define MAX_LENGTH 3

struct timeval gtodTimes[N];
//char *procClockTime[N];
char procClockTimes[N][800];
char data[150];

int main()
{
	int i;
	ssize_t bytes_read;		//Holds value returned by read() system call
	int fd = open("/dev/mytime", O_RDONLY);	//Open the character device file
	if(fd < 0)		//If unable to open character device
	{
		printf("\nError opening file /dev/mytime");
		return -1;
	}
	for(i=0;i<N;i++)
	{
		gettimeofday(&gtodTimes[i], NULL);
		bytes_read = read(fd, procClockTimes[i], sizeof(procClockTimes));
		if(bytes_read < 0)
		{
			printf("\nError, unable to read from /dev/mytime");
			return -1;
		}
	}
	//bytes_read = read(fd, data, sizeof(data));
	close(fd);
	for(i=0;i<N;i++)
	{
		printf("\nTime obtained using gettimeofday() user level library: %ld.%ld", gtodTimes[i].tv_sec,gtodTimes[i].tv_usec);
		printf("\nTime obtained using kernel module: %s\n", procClockTimes[i]);
	}
	return 0;	
}
