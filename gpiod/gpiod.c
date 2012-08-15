/* device driver: gpiod  */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include <linux/sched.h>

#include <asm/mach/arch.h>

#include "gpiod.h"
 
static dev_t gpiod; 
static struct class *dev_class; 

#define GPIOD_VERSION   1

#define DEV_NAME "gpiod"
#define CLASS_NAME "gpiod_dev"

#define GPIOD_STATE_WAIT 0
#define GPIOD_STATE_ACTIVE 1
struct Gpiod_data
{
        wait_queue_head_t m_wait_queue;
        atomic_t m_state;  
        struct semaphore m_sem; 
        struct cdev m_cdev;
};

static struct Gpiod_data the_gpiod_data;
static avi_plugin_work_t the_plugin;

static int waitup_gpiod_device(void)
{
   struct Gpiod_data *pdata = &the_gpiod_data;

   //printk("To wait up gpiod device\n");
   
   if (down_interruptible(&pdata->m_sem))return -ERESTARTSYS;

   atomic_set(&the_gpiod_data.m_state, GPIOD_STATE_ACTIVE);
   
   up(&pdata->m_sem);

   wake_up_interruptible(&pdata->m_wait_queue);   
   return 0;
}

static int avi_button_press(void)
{
	int ret = 0;
	//printk("%s: message from interrupt\n",__FUNCTION__);
	waitup_gpiod_device();
	return ret;
}

static int dev_open(struct inode *pinode, struct file *filp)
{
   struct Gpiod_data *pdata;

   pdata = container_of(pinode->i_cdev, struct Gpiod_data, m_cdev);
   filp->private_data = pdata;

   //printk(KERN_INFO "Gpiod Driver: open()\n");
   return 0;
}

static int dev_close(struct inode *pinode, struct file *filp)
{
   //printk(KERN_INFO "Gpiod Driver: close()\n");
   return 0;
}

static ssize_t dev_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
   struct Gpiod_data *pdata = filp->private_data;
   
   //printk(KERN_INFO "Gpiod Driver: read()\n");

   if (down_interruptible(&pdata->m_sem))return -ERESTARTSYS;

   atomic_set(&the_gpiod_data.m_state, GPIOD_STATE_WAIT);
   
   up (&pdata->m_sem);

   wake_up_interruptible(&pdata->m_wait_queue);
   return 0;
}

static ssize_t dev_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
   //printk(KERN_INFO "Gpiod Driver: write()\n");
   return len;
}


int dev_ioctl(struct inode *pinode, struct file *filp, unsigned int cmd, unsigned long arg)
{
   //printk("<1>dev ioctl, cmd: 0x%x, arg: %lx.\n", cmd, arg);

   switch (cmd) 
   {
      case GPIOD_SET_MODE:
         //printk("ioctl: [%d], set mode[%ld]\n", cmd, *(unsigned long *)arg);
         waitup_gpiod_device();
         break;
      case GPIOD_SERVICE:
         //printk("ioctl: [%d], service[%ld]\n", cmd, *(unsigned long *)arg);
         break;
      default:
         break;
   }

   return 0;
}

static unsigned int dev_poll(struct file *filp, poll_table *wait)
{
   struct Gpiod_data *pdata = filp->private_data;
   unsigned int mask = 0;

   down(&pdata->m_sem);
   poll_wait(filp, &pdata->m_wait_queue,  wait);
   if (atomic_read(&pdata->m_state) == GPIOD_STATE_ACTIVE)
   {
      mask |= POLLIN | POLLRDNORM;
   }      
   up(&pdata->m_sem);
   return mask;
}

static struct file_operations gpiod_fops =
{
  .owner = THIS_MODULE,
  .open = dev_open,
  .release = dev_close,
  .read = dev_read,
  .write = dev_write,
  .ioctl = dev_ioctl,
  .poll = dev_poll,
};
 
static int __init gpiod_init(void) /* Constructor */
{
   //printk(KERN_INFO "Driver init: gpiod registered");

   memset((void *)&the_gpiod_data,0, sizeof(the_gpiod_data));
   init_waitqueue_head(&(the_gpiod_data.m_wait_queue));
   init_MUTEX(&the_gpiod_data.m_sem);
   atomic_set(&the_gpiod_data.m_state, GPIOD_STATE_WAIT);
   
   if (alloc_chrdev_region(&gpiod, 0, 1, DEV_NAME) < 0)
   {
      return -1;
   }
   
   if ((dev_class = class_create(THIS_MODULE, CLASS_NAME)) == NULL)
   {
      unregister_chrdev_region(gpiod, 1);
      return -1;
   }
   
   if (device_create(dev_class, NULL, gpiod, NULL, DEV_NAME) == NULL)
   {
      class_destroy(dev_class);
      unregister_chrdev_region(gpiod, 1);
      return -1;
   }
   
   cdev_init(&the_gpiod_data.m_cdev, &gpiod_fops);
   the_gpiod_data.m_cdev.owner = THIS_MODULE;
   if (cdev_add(&the_gpiod_data.m_cdev, gpiod, 1) == -1)
   {
      device_destroy(dev_class, gpiod);
      class_destroy(dev_class);
      unregister_chrdev_region(gpiod, 1);
      return -1;
   }

   the_plugin.m_func = avi_button_press;
   the_plugin.m_next = 0;
   register_avi_button_work(&the_plugin);
   
   printk("<1>Inserting gpiod module, version[%d]\n",GPIOD_VERSION);
   return 0;
}
 
static void __exit gpiod_exit(void) /* Destructor */
{
  cdev_del(&the_gpiod_data.m_cdev);
  device_destroy(dev_class, gpiod);
  class_destroy(dev_class);
  unregister_chrdev_region(gpiod, 1);
  unregister_avi_button_work(&the_plugin);
  printk(KERN_INFO "Driver exit: gpiod unregistered\n");
}
 
module_init(gpiod_init);
module_exit(gpiod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bin Liu");
MODULE_DESCRIPTION("Linux Character Device Driver");

