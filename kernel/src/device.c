#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <asm/uaccess.h>        /* for put_user */

/* local headers */
#include <device.h>

static int dev_open = 0;
static char msg[DEV_BUF_LEN];
static char *msg_Ptr;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static struct miscdevice fmon_dev = {
	/*
	 * We don't care what minor number we end up with,
	 * so tell the kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	DEVICE_NAME,

	/*
	 * What functions to call when a program performs
	 * file operations on the device.
	 */
	&fops
};

static int
device_open(struct inode *inode, struct file *file)
{
	static int counter = 0;

	if (dev_open)
		return -EBUSY;

	dev_open++;
	sprintf(msg, "I already told you %d times Hello world!\n", counter++);
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	return 0;
}

/*
 * Called when a process closes the device file.
 */
static int
device_release(struct inode *inode, struct file *file)
{
	dev_open--;

	/*
	 * Decrement the usage count, or else once
	 * you opened the file, you'll never get get
	 * rid of the module.
	 */
	module_put(THIS_MODULE);
	return 0;
}

/*
 * Called when a process, which already opened the
 * dev file, attempts to read from it.
 */
static ssize_t
device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	int bytes_read = 0;

	/*
	 * If we're at the end of the message,
	 * return 0 signifying end of file
	 */
	if (*msg_Ptr == 0)
		return 0;

	/*
	 * Actually put the data into the buffer
	 */
	while (length && *msg_Ptr) {
		/*
		 * The buffer is in the user data segment, not the kernel
		 * segment so "*" assignment won't work.  We have to use
		 * put_user which copies data from the kernel data segment to
		 * the user data segment.
		 */
		put_user(*(msg_Ptr++), buffer++);

		length--;
		bytes_read++;
	}

	return bytes_read;
}

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
	return -EINVAL;
}

int
register_fmon_dev(void)
{
	int ret;

	/*
	 * Create the 'fmon' device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/fmon device using
	 * the default rules.
	 */
	ret = misc_register(&fmon_dev);
	if (ret)
		printk(KERN_ERR "Unable to register misc device.\n");

	return ret;
}

void
unregister_fmon_dev(void)
{
	misc_deregister(&fmon_dev);
}
