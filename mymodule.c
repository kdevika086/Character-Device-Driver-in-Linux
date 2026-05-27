#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/version.h>

#define DEVICE_NAME "mychardev"

//global variables for user input
static int kernel_version[2];
static int time_limit;


//module parameters to pass input when kernel loads
module_param_array(kernel_version, int, NULL, 0);
module_param(time_limit, int, 0);


static dev_t dev_num;

// cdev struct: kernel structure to represent character device
struct cdev my_chardev;

static int read_flag=0;
static int write_flag=0;
static char uname[50];

static unsigned long start_time;


//read function for device
//called when "cat /dev/mychardev"

//ssize_t: returns no of butes read if success else negativ error code
//__user: pointer coming from user space
//loff_t *off: file offset acts as cursor showing how much read
static ssize_t my_read(struct file* file, char __user *buf, size_t len, loff_t *off) 
{
	read_flag=1;
	printk(KERN_INFO "Device read completed successfully\n");
	return 0;
}

//write function for device

static ssize_t my_write(struct file* file, const char __user *buf, size_t len, loff_t *off)
{
	if(read_flag!=1)
	{
		printk(KERN_ALERT "Write attempted before read\n");
		return -EINVAL;
	}
	//size check to avoid buffer overflow
	if(len>sizeof(uname))
	{
		len=sizeof(uname);
	}
	//copy_from_user: returns 0 is successful else returns npo of bytes not read
	if(copy_from_user(uname, buf, len))
	{
		return -EFAULT;
	}
	uname[len-1]='\0';
	write_flag=1;
	printk(KERN_INFO "Username recieved: %s\n", uname);
	return len;
}

//change file operations
static struct file_operations fop =
{
	.read= my_read,
	.write =my_write,
	.owner=THIS_MODULE,
};


//init function
static int __init my_init(void)
{
	int comp_major, comp_minor, major, minor;
	//extract kernel version
	comp_major=(LINUX_VERSION_CODE >>16) & 0xFF;
	comp_minor=(LINUX_VERSION_CODE >>8)& 0xFF;
	
	//check version
	if(kernel_version[0]!=comp_major || kernel_version[1]!=comp_minor)
	{
		printk(KERN_ALERT "Kernel versions not matching\n");
		return -EINVAL;
	}
	
	//allocate device
	if(alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME)<0)
	{
		printk(KERN_ALERT "Device allocation failed\n");
		return -1;
	}
	major=MAJOR(dev_num);
	minor=MINOR(dev_num);
	
	cdev_init(&my_chardev, &fop);
	if(cdev_add(&my_chardev,dev_num, 1)<0)
	{
		printk(KERN_ALERT "cdev add failed\n");
		unregister_chrdev_region(dev_num,1);
		return -1;
	}
	
	//start time
	start_time=jiffies;
	printk(KERN_INFO "Device registered, Major=%d, Minor=%d, Time=%d\n",major,minor, time_limit);
	return 0;
}
	
static void __exit my_exit(void)
{
	unsigned long elapsed_time;
	elapsed_time=(jiffies-start_time)/HZ;
	if(read_flag && write_flag && elapsed_time<=time_limit)
	{
		printk(KERN_INFO "Successfully completed within time limit, User:%s\n",uname);
	}
	else
	{
		printk(KERN_ALERT "Failure: all conditions not satisfied\n");
	}
	cdev_del(&my_chardev);
	unregister_chrdev_region(dev_num,1);
}

module_init(my_init);
module_exit(my_exit);


MODULE_LICENSE("GPL");
























