#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/preempt.h>
#include <linux/delay.h>


void tasklet_handler(unsigned long data)
{
	pr_info("%s work\n", __func__);
	pr_info("tasklet: in_interrupt(): 0x%0lx\n", in_interrupt());
	pr_info("tasklet: in_irq(): 0x%0lx\n", in_irq());
	pr_info("tasklet: in_softirq(): 0x%0lx\n", in_softirq());
	pr_info("tasklet: in_nmi(): 0x%0lx\n", in_nmi());

	msleep(20);
}

DECLARE_TASKLET(my_tasklet, tasklet_handler, 0);

static int __init hello_init(void)
{
	pr_info("module init : in_interrupt(): 0x%0lx\n", in_interrupt());
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
