#include<linux/linkage.h>
#include<linux/export.h>
#include<linux/time.h>
#include<linux/time64.h>
#include<linux/uaccess.h>
#include<linux/printk.h>
#include<linux/slab.h>

asmlinkage long sys_my_xtime(struct timespec *current_time){
	struct timespec ts = current_kernel_time();					//Returns current time
	
        if(!access_ok(VERIFY_WRITE, current_time, sizeof(current_time)))		//Checks if user-space is writeable
	{
		return -EFAULT;
	}
	current_time->tv_sec = ts.tv_sec;						//tv_sec stores current time in seconds
	current_time->tv_nsec = (ts.tv_sec * 1000000000) + ts.tv_nsec;			//tv_nsec stores current time in nanoseconds
	printk(KERN_ALERT "Time in nanoseconds: %ld", current_time->tv_nsec);		
	printk(KERN_ALERT "Time in seconds: %ld", current_time->tv_sec);
	return 0;
}
EXPORT_SYMBOL(sys_my_xtime);
