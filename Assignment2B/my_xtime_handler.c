#include <linux/linkage.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
asmlinkage int sys_my_xtime(void){
	printk("Time in nanoseconds is: %ld", timespec.tv_nsec);
	return 0;
}
EXPORT_SYMBOL(sys_my_xtime);
