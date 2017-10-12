#include<stdio.h>
#include<unistd.h>
#include<linux/unistd.h>
#include<linux/time.h>
int main(){
        struct timespec ts;			
        long int return_value=syscall(327,&ts);				//Calling our system call
	printf("Syscall return value: %ld\n", return_value);		//Prints 0 if success, else -1
        printf("Time in seconds: %ld\n", ts.tv_sec);			
	printf("Time in nanoseconds: %ld\n", ts.tv_nsec);
        return 0;
}
