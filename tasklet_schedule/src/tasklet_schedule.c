#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>


void tasklet_handler(unsigned long data)
{
	pr_info("%s work\n", __func__);
}

DECLARE_TASKLET(my_tasklet, tasklet_handler, 0);

static int __init hello_init(void)
{
	tasklet_schedule(&my_tasklet);
	return 0;
}

static void __exit hello_exit(void)
{
	tasklet_kill(&my_tasklet);
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");
