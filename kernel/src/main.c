#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/smp_lock.h>

static int __init
fmon_init(void)
{

	/* heart of the module */

	return 0;
}

static void __exit
fmon_exit(void)
{
	/* clean up everything */
}

module_init(fmon_init);
module_exit(fmon_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Uladzislau Rezki");
