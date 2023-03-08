#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/proc_fs.h>

static dev_t first;
static struct cdev c_dev; 
static struct class *cl;

static struct proc_dir_entry *entry;

static char summ_str[1024*16];
static size_t end_char;

static bool is_digit(char ch)
{
    return (ch >= '0' && ch <= '9');
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{    
    /*printk(KERN_INFO "Driver: %d\n", summ);
    char buffer[15];

    int size = sprintf(buffer, "%d\n", summ);

    if (*off > 0 || len < size)
	{
		return 0;
	}


    if (copy_to_user(buf, buffer, size) != 0)
	{
		return -EFAULT;
	}

    *off = len;
    return size;*/

    size_t size = end_char + 1;

    if (*off > 0 || len < size) 
    {
        return 0;
    }

    if (copy_to_user(buf, summ_str, size) != 0)
	{
		return -EFAULT;
	}

    *off = len;
    return size;
}

static ssize_t my_write(struct file *f, const char __user *buf,  size_t len, loff_t *off)
{
    char string[len];
    if (copy_from_user(string, buf, len) != 0) {
        return -EFAULT;
    }


    long summ = 0;
    size_t is_neg = false;
    long tmp_summ = 0;
    size_t i = 0;
    while (i < len)
    {
        if (string[i] == '-')
            is_neg = true;

        else if (is_digit(string[i]))
        {
            tmp_summ *= 10;
            tmp_summ += string[i] - '0';
        }

        else
        {
            if (is_neg)
                tmp_summ *= -1;
            summ += tmp_summ;
            tmp_summ = 0;
            is_neg = false;
        }
        i++;
    }

    if (is_neg)
        tmp_summ *= -1;
    summ += tmp_summ;

    end_char += sprintf(summ_str + end_char, "%d\n", summ);    
    summ_str[end_char] = '\0';

    return len;
}

static int my_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static struct file_operations mychdev_fops =
{
  .owner      = THIS_MODULE,
  .read       = my_read,
  .write      = my_write
};

static const struct proc_ops proc_fops = { 
	.proc_read 	= my_read
};

static int __init ch_drv_init(void)
{
    end_char = 0;
    summ_str[end_char] = '\0';

    printk(KERN_INFO "Hello!\n");

    //create file in dev
    if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0)
    {
		return -1;
    }

    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
		unregister_chrdev_region(first, 1);
		return -1;
    }

    cl->dev_uevent = my_dev_uevent;

    if (device_create(cl, NULL, first, NULL, "var3") == NULL)
    {
		class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    
    cdev_init(&c_dev, &mychdev_fops);
    if (cdev_add(&c_dev, first, 1) == -1)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }

    //create file in proc
    entry = proc_create("var3", 0666, NULL, &proc_fops);
    if (entry == NULL) {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        cdev_del(&c_dev);
        return -1;
    }

    return 0;
}
 
static void __exit ch_drv_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    proc_remove(entry);
    printk(KERN_INFO "Bye!!!\n");
}
 
module_init(ch_drv_init);
module_exit(ch_drv_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonina");
MODULE_DESCRIPTION("Character device driver");

