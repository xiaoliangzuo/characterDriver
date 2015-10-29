#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define GLOBALMEM_SIZE 50000
#define MEMCLEAR 0X1

/*the devices major 254 */
#define GLOBAL_MAJOR 254

static int global_major = GLOBAL_MAJOR;

typedef struct globalmem_dev{
	struct cdev dev;
	unsigned char memory[GLOBALMEM_SIZE];
} GLOBALDEV;

static GLOBALDEV dev;
static atomic_t atomic_available = ATOMIC_INIT(1);

static ssize_t globaldev_read(struct file *filp, char __user *buf, size_t count, loff_t *opps)
{
	unsigned  long p = *opps;
	if(p > GLOBALMEM_SIZE)
	{
//		printk(KERN_INFO "ERROR %d is more than the memory", p);

//		return -EFAULT;
	}
	if(copy_to_user(buf, (void*)(dev.memory + p), count))
	{
		return -EFAULT;
	}
	else
	{
		*opps += count ;
		printk(KERN_INFO "read %d bytes from %d", count, p);
		return count;
	}
	
	return 0;
}


static ssize_t globaldev_write(struct file *filp, char __user *buf, size_t count, loff_t *opps)
{
	unsigned  long p = *opps;
	if(p > GLOBALMEM_SIZE)
	{
//		printk(KERN_INFO "ERROR %d is more than the memory in the write", p);
		return -ENXIO;
	}
	if(copy_from_user((void *)(dev.memory + p), buf,  count))
	{
		return -EFAULT;
	}
	else
	{
		*opps += count ;
		printk(KERN_INFO "write %d bytes from %d", count, p);
		return count;
	}
	
	return 0;
}

static int globaldev_open(struct inode* inode,struct file *filp)
{
	if(!atomic_dec_and_test(&atomic_available))
	{
		atomic_inc(&atomic_available);
		printk(KERN_INFO"DEVICE IS USED ,OPEN ERROR\N");
		return -EBUSY;
	}
	return 0;
}

static int globaldev_release(struct inode* inode,struct file *filp)
{
	atomic_inc(&atomic_available);
	return 0;
}


static int globaldev_llseek(struct file *filp, loff_t offset, int orig)
{
	unsigned int ret = 0;
	switch(orig)
	{
		case 0:
			filp->f_pos = offset;
			break;
		case 1:
			filp->f_pos += offset;
			break;
		default:
			break;
	}

	return filp->f_pos;


}



static const struct file_operations globaldev_ops ={
	.owner  = THIS_MODULE,
	.open   = globaldev_open,
	.llseek = globaldev_llseek,
	.read   = globaldev_read,
	.write  = globaldev_write,
/*	.ioctl  = globaldev_ioctl,*/
	.release= globaldev_release,
};

int globaldev_init(void)
{
	int result = 0;
	dev_t devnum ;

	result = alloc_chrdev_region(&devnum, 0, 1, "globaldev");
 	if(result < 0) {
		return result;
	}
	
	cdev_init(&(dev.dev), &globaldev_ops);
	dev.dev.owner = THIS_MODULE;
	dev.dev.ops   =  &globaldev_ops;

	result = cdev_add(&(dev.dev), devnum, 1);

//	if(result) 
//		printk(KERN_NOTICE "ERROR %d init ", result);
	
	return 0;
}

void globaldev_exit(void)
{
	cdev_del(&(dev.dev));
	unregister_chrdev_region( dev.dev.dev, 1);
}

MODULE_AUTHOR("FF");
MODULE_LICENSE("Dual BSD/GPL");
module_init(globaldev_init);
module_exit(globaldev_exit);














