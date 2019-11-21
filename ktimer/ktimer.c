#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

static void my_tasklet_handler(unsigned long flag);
DECLARE_TASKLET(my_tasklet, my_tasklet_handler, 0);

static void my_tasklet_handler(unsigned long flat)
{
	ktime_t time;
	int val=1, cnt, i=0;

	//tasklet_disable(&my_tasklet);

	pr_info("ktime test for profiling\n");

	time = ktime_get();

	for (cnt=0; cnt<10000; cnt++) {
		for (i=0; i<10; i++)
			val *= 2;
	}

	time = ktime_sub(ktime_get(), time);
	pr_info("2^10 = %d, spend time: %lld, iterations: %d\n",
		val, ktime_to_ns(time), cnt);	

	//tasklet_enable(&my_tasklet);
}

static int __init hello_init(void)
{
	pr_info("module init start\n");
	tasklet_schedule(&my_tasklet);	
	pr_info("module init end\n");
	return 0;
}

static void __exit hello_exit(void)
{
	tasklet_kill(&my_tasklet);
	pr_info("How're you doing\n");
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");
