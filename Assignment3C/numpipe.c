#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <asm/errno.h>
#include <linux/moduleparam.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>

MODULE_LICENSE("DUAL BSD/GPL");

int buffer_size;
module_param(buffer_size, int, 0);
char *buffer;
static int Device_Open = 0, no_of_writes = 0;

DEFINE_SEMAPHORE(empty);
DEFINE_SEMAPHORE(full);
DEFINE_SEMAPHORE(mutex);

//Function declarations
static int my_open(struct inode *, struct file *);
static ssize_t my_write(struct file *, const char *, size_t, loff_t *);
//static int my_close(struct inode *, struct file *);
static ssize_t my_read(struct file *, char *, size_t, loff_t *);
static int __init my_module_init(void);

//File operations data structure
static struct file_operations my_fops =
{
        .owner = THIS_MODULE,
        .read = my_read,
        .open = my_open,
        //.release = my_close,
	.write = my_write,
        .llseek = noop_llseek
};

static struct miscdevice my_misc_device =
{
        .minor = MISC_DYNAMIC_MINOR,
        .name = "numpipe",
        .fops = &my_fops
};

static int __init my_module_init(void)
{
	int error;
	error = misc_register(&my_misc_device);

	if(error)
	{
		printk("\nRegistering the character device failed with error number: %d", error);
		return error;
	}
	printk(KERN_ALERT "Character device successfully registered! Buffer size = %d!", buffer_size);
	
	//Setting the semaphores
	sema_init(&empty, buffer_size);
	sema_init(&full, 0);
	sema_init(&mutex, 1);

	buffer = kmalloc(buffer_size, GFP_KERNEL);
	
	return 0;	
}

static void __exit my_module_exit(void)
{
	misc_deregister(&my_misc_device);			//Deregisters character device
	printk(KERN_ALERT "Character device de-registered! Buffer removed");
}

//Runs when a process opens the /dev/mytime file
static int my_open(struct inode *inode, struct file *file)
{
	/*if(Device_Open)				//If another process is using this file
	{
		return -EBUSY;
	}
	*/
	Device_Open++;
	return 0;
}

static ssize_t my_write(struct file *filep, const char *buf, size_t len, loff_t *offset)
{
	//int bytes;
	int buffer_pointer = 0, bytes_written = 0;
	int return_value = 0;
	while(buffer_pointer < len)
	{
		return_value = down_interruptible(&empty);
		if(return_value < 0)			//Decrement empty semaphore, to add items into buffer
		{
			printk(KERN_ALERT "Interrupt signal received while changing semaphore: %d, producer terminated", return_value);
			return -1;
		}
		return_value = down_interruptible(&mutex);
		if(return_value < 0)			//Acquire mutex to execute critical section
		{
			printk(KERN_ALERT "Interrupt signal received while acquiring mutex: %d, producer terminated", return_value);
			return -1;
		}
		
		if(copy_from_user((buffer + no_of_writes), (buf + buffer_pointer), 1) < 0)
		{
			printk(KERN_ALERT "Could not copy from user");
			return -1;
		}
		
		bytes_written++;
		printk(KERN_ALERT "Some data written to buffer");
		no_of_writes++;
		buffer_pointer++;
		up(&mutex);
		up(&full);
	}
	return bytes_written;
}

static ssize_t my_read(struct file *filep, char *buf, size_t len, loff_t *offset)
{
	int buffer_pointer = 0, bytes_read = 0, return_value, i;
	while(buffer_pointer < len)
	{
		return_value = down_interruptible(&full);	//Decrement full semaphore, to remove items from buffer
                if(return_value < 0)
                {
                        printk(KERN_ALERT "Interrupt signal received while changing semaphore: %d, consumer terminated", return_value);
                        return -1;
		}
		return_value = down_interruptible(&mutex);	//Acquire mutex to execute critical section
		if(return_value < 0)
		{
			printk(KERN_ALERT "Interrupt signal received while acquiring mutex: %d, consumer terminated", return_value);
			return -1;
		}
		if(copy_to_user((buf+buffer_pointer), buffer, 1) < 0)	//Executing critical section
		{
			printk(KERN_ALERT "Could not copy to user");
			return -1;
		}
		
		bytes_read++;
		printk(KERN_ALERT "Some data read from buffer");

		for( i=0 ; i < no_of_writes - 1 ; i++ )		//Shifting the buffer
		{
			buffer[i] = buffer[i+1];
		}
		
		no_of_writes = no_of_writes - 1;		//Removing data
		up(&mutex);			//Release mutex after exiting critical section
		up(&empty);			//Incrementing empty semaphore as data has been removed from buffer
		buffer_pointer++;
	}
	return bytes_read;
}

module_init(my_module_init)
module_exit(my_module_exit)
