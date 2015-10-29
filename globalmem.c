/*


 */
#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/completion.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define GLOBALMEM_SIZE 50000
#define MEMCLEAR 0X1
#define MCHINE_NUM 4

/*the devices major 254 */
#define GLOBAL_MAJOR 254

static int global_major = GLOBAL_MAJOR;

static unsigned long *order_num;  // the zhizhen is used to cunchu ordernum

typedef struct globalmem_dev{
	struct cdev dev;
	unsigned char memory[GLOBALMEM_SIZE];
	unsigned long order_num; //the order num of the machine  you use
} GLOBALDEV;

static GLOBALDEV *dev;

DECLARE_COMPLETION(my_comple1);
DECLARE_COMPLETION(my_comple2);
static int flag = 1;
static ssize_t globaldev_read(struct file *filp, char __user *buf, size_t count, loff_t *opps)
{
	unsigned  long p = *opps;
	int count1 = dev->order_num;
	if(p > GLOBALMEM_SIZE)
	{
		printk(KERN_INFO "ERROR %d is more than the memory", p);

	 	return -EFAULT;
	}

	wait_for_completion(&my_comple1);
	init_completion(&my_comple1);

	if(copy_to_user(buf, (void*)(dev[count1].memory + p), count))
	{
		return -EFAULT;
	}
	else
	{
		*opps += count ;
		printk(KERN_INFO "read %d bytes from %d", count, p);
    complete(&my_comple2);	
		return count;
	}
	return 0;
}


static ssize_t globaldev_write(struct file *filp, char __user *buf, size_t count, loff_t *opps)
{
	unsigned  long p = *opps;
	int count1 = dev->order_num;
	if(p > GLOBALMEM_SIZE)
	{
//		printk(KERN_INFO "ERROR %d is more than the memory in the write", p);
		return -ENXIO;
	}
	printk(KERN_INFO"FLAG IS %d\n",flag);
	if(!flag)
	{
		wait_for_completion(&my_comple2);

	    init_completion(&my_comple2);
	}else {
		flag = 0;
	}
	printk(KERN_INFO"start to write\n");
	if(copy_from_user((void *)(dev[count1].memory + p), buf,  count))
	{  
		printk(KERN_INFO"write error\n");
		return -EFAULT;
	}
	else
	{
		*opps += count ;
		printk(KERN_INFO "write %d bytes from %d", count, p);

		complete(&my_comple1);
		return count;
	}
	//complete(&my_comple1);
	return 0;
}

static int globaldev_open(struct inode* inode,struct file *filp)
{

	unsigned long  i = 0;
//	INIT_COMPLETION(&my_comple1);
//	INIT_COMPLETION(&my_comple2);
	for(i = 0; i < MCHINE_NUM; i++)
	{
		unsigned long  res = test_bit(i,(void *)order_num);
		if(res == 0)  //this mchine is not used,i can use it 
		{
			dev->order_num = i;
			change_bit(i, (void *)order_num);
			break;
		}
	}
	if(i == MCHINE_NUM) {
		printk(KERN_INFO"no device can be used\n");
		return -EFAULT;
	}
	dev->order_num = 0;
	printk(KERN_INFO"NOW I am using the %d devcie for the test\n",dev->order_num);
	return 0;
}

static int globaldev_release(struct inode* inode,struct file *filp)
{
	clear_bit(dev->order_num, (void *)order_num);
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
	int i, result = 0;
	dev_t devnum ;
	
	result = alloc_chrdev_region(&devnum, 0, MCHINE_NUM, "globaldev");
 	if(result < 0) {
		return result;
	}
	order_num = kmalloc( 4 * sizeof(unsigned long), GFP_KERNEL);
	dev = kmalloc( MCHINE_NUM * sizeof(GLOBALDEV), GFP_KERNEL);
	if(!dev)
	{
		printk(KERN_INFO"Kmalloc error\n");
	}
	for(i = 0; i < MCHINE_NUM; i++)
	{
		cdev_init(&(dev[i].dev), &globaldev_ops);
		dev[i].dev.owner = THIS_MODULE;
		dev[i].dev.ops   =  &globaldev_ops;
	
		result = cdev_add(&(dev[i].dev), MKDEV( MAJOR(devnum), i) , 1);
	
		if(result) 
			printk(KERN_NOTICE "ERROR %d init %d ", result, i);
	}	
	printk(KERN_INFO"INIT OK\n");
	return 0;
}

void globaldev_exit(void)
{
	int i ;
	for(i = 0; i < MCHINE_NUM; i++)
		cdev_del(&(dev[i].dev));

	kfree(order_num);
	unregister_chrdev_region( MKDEV(250, 0), MCHINE_NUM);
}

MODULE_AUTHOR("shawn");
MODULE_LICENSE("Dual BSD/GPL");
module_init(globaldev_init);
module_exit(globaldev_exit);














