#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

static void my_tasklet_handler(unsigned long flag);
DECLARE_TASKLET(my_tasklet, my_tasklet_handler, 0);

static void my_tasklet_handler(unsigned long flat)
{
	int val, cnt, i;
	int count=1000;
	unsigned long long total = 0;
	ktime_t t1, t2;

	pr_info("ktime test for profiling\n");


	for (cnt=0; cnt<count; cnt++) {
		t1 = ktime_get();
		val = 1;
		for (i=0; i<10; i++)
			val *= 2;
		t2 = ktime_get();
		total += ktime_to_ns(ktime_sub(t2, t1));
	}

	pr_info("2^10 = %d, spend time: %lld, iterations: %d\n",
		val, total/count, count);	
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
