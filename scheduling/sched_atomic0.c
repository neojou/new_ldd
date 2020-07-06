#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>	/* for sleep */
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>		/* for kmalloc */
#include <linux/workqueue.h>
#include <linux/spinlock.h>


struct workqueue_struct *wq[8];
struct work_struct work[8];

int sum = 0;
const int counts=100;

DEFINE_SPINLOCK(my_lock);

static void producer(struct work_struct *work)
{
	int i;
	volatile int val;

	pr_info("producer: start\n");
	for (i=0; i<counts; i++) {
		val = sum;
		val = val + 1;
		schedule();
		sum = val;
	}
	pr_info("producer: finished, i=%d\n", i);
}

static void consumer(struct work_struct *work)
{
	int i;
	volatile int val;

	pr_info("consumer: start\n");
	for (i=0; i<counts; i++) {
		val = sum;
		val = val - 1;
		schedule();
		sum = val;
	}
	pr_info("consumer: finished, i=%d\n", i);
}

static int __init hello_init(void)
{
	wq[0] = create_singlethread_workqueue("producer wq");
	wq[1] = create_singlethread_workqueue("consumer wq");

	pr_info("%s: pid=%d, before sum=%d\n", __func__, current->pid, sum);
	INIT_WORK(&work[0], producer);
	INIT_WORK(&work[1], consumer);
	queue_work(wq[0], &work[0]);
	queue_work(wq[1], &work[1]);
	flush_workqueue(wq[0]);
	flush_workqueue(wq[1]);

	pr_info("%s: pid=%d, after sum=%d\n", __func__, current->pid, sum);
	return 0;
}

static void __exit hello_exit(void)
{
	cancel_work_sync(&work[0]);
	cancel_work_sync(&work[1]);
	destroy_workqueue(wq[0]);
	destroy_workqueue(wq[1]);
}


module_init(hello_init);
module_exit(hello_exit);



MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");

