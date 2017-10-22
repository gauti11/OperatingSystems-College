#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <asm/errno.h>

MODULE_LICENSE("DUAL BSD/GPL");
//Function declarations
static int my_open(struct inode *, struct file *);
static int my_close(struct inode *, struct file *);
static ssize_t my_read(struct file *, char *, size_t, loff_t *);
static int __init my_module_init(void);

#define DEVICE_NAME "mytime"
#define BUFFER_LEN 100

static int Device_Open = 0;				//Indicates whether device is open or not

//File operations data structure
static struct file_operations my_fops =
{
        .owner = THIS_MODULE,
        .read = my_read,
        .open = my_open,
        .release = my_close,
        .llseek = noop_llseek
};

static struct miscdevice my_misc_device =
{
        .minor = MISC_DYNAMIC_MINOR,
        .name = "mytime",
        .fops = &my_fops
};

//Runs when kernel module is loaded (when 'insmod' command is executed)
static int __init my_module_init(void)
{
	int error;
	error = misc_register(&my_misc_device);
	if(error)
	{
		printk("\nRegistering the character device failed with error number: %d", error);
		return error;
	}
	printk(KERN_ALERT "Character device successfully registered!");	
	return 0;	
}

//Runs when kernel module is removed (when 'rmmod' command is executed)
static void __exit my_module_exit(void)
{
	misc_deregister(&my_misc_device);			//Deregisters character device
	printk(KERN_ALERT "Character device de-registered!");
}

//Runs when a process opens the /dev/mytime file
static int my_open(struct inode *inode, struct file *file)
{
	if(Device_Open)				//If another process is using this file
	{
		return -EBUSY;
	}
	Device_Open++;
	return 0;
}

//Runs when a process closes the /dev/mytime file
static int my_close(struct inode *inode, struct file *file)
{
	Device_Open--;				//Keeps count of times file is closed
	return 0;
}

//Runs when a process tries to read from /dev/mytime file
static ssize_t my_read(struct file *file, char *out, size_t length, loff_t *offset)
{
	size_t bytes_read = 0;
	char buf[120];				//Stores data to be copied to user space
	int len = 0;
	//struct timespec ts1 = current_kernel_time();
	//struct timespec ts2;			
	//getnstimeofday(&ts2);			//Different timing mechanisms
	struct timespec ts1, ts2;
	getnstimeofday(&ts2);
	ts1 = current_kernel_time();
	
	if(!access_ok(VERIFY_WRITE, out, sizeof(out)))		//Checks if user space is writable
	{
		return -EFAULT;
	}
	len = sprintf(buf, "\nCurrent_kernel_time: %ld.%ld\ngetnstimeofday: %ld.%ld\n", ts1.tv_sec, ts1.tv_nsec, ts2.tv_sec, ts2.tv_nsec);						//Set the data to be passed to user space
	//if(*offset == 0)
	//{
		if(copy_to_user(out,buf,sizeof(buf)+1))		//Returns -1 on success
		{
			bytes_read = -EFAULT;
		}
		else
		{
			bytes_read = sizeof(buf);
			//*offset = 1;
		}
	//}
	return bytes_read;
}

module_init(my_module_init)
module_exit(my_module_exit)
