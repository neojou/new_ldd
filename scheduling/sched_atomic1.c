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
const int counts=10;

DEFINE_SPINLOCK(my_lock);

static inline void check_atomic(void)
{
	if (preempt_count()) {
		pr_info("preempt_count():0x%x\n", preempt_count());
		pr_info("in_atomic():0x%x\n", in_atomic());
		pr_info("in_atomic_preempt_off():0x%x\n", in_atomic_preempt_off());
	}
}

static void work_handler1(struct work_struct *work)
{
	int i;
	volatile int val;

	pr_info("%s: start\n", __func__);
	for (i=0; i<counts; i++) {
		spin_lock(&my_lock);
		check_atomic();
		val = sum;
		val = val + 1;
		sum = val;
		msleep(1);
		spin_unlock(&my_lock);
	}
	pr_info("%s: finish\n", __func__);
}

static void work_handler2(struct work_struct *work)
{
	int i;
	volatile int val;

	pr_info("%s: start\n", __func__);
	for (i=0; i<counts; i++) {
		spin_lock(&my_lock);
		check_atomic();
		val = sum;
		val = val - 1;
		sum = val;
		spin_unlock(&my_lock);
	}
	pr_info("%s: finish\n", __func__);
}

static int __init hello_init(void)
{
	int i;

	for (i=0; i<8; i++)
		wq[i] = create_singlethread_workqueue("work queue");

	pr_info("%s: pid=%d, before sum=%d\n", __func__, current->pid, sum);

	for (i=0; i<8; i+=2)
		INIT_WORK(&work[i], work_handler1);
	for (i=1; i<8; i+=2)
		INIT_WORK(&work[i], work_handler2);
	for (i=0; i<8; i++)
		queue_work(wq[i], &work[i]);
	for (i=0; i<8; i++)
		flush_workqueue(wq[i]);

	pr_info("%s: pid=%d, after sum=%d\n", __func__, current->pid, sum);
	return 0;
}

static void __exit hello_exit(void)
{
	int i;

	for (i=0; i<8; i++)
		cancel_work_sync(&work[i]);
	for (i=0; i<8; i++)
		destroy_workqueue(wq[i]);
	pr_info("%s: bye\n", __func__);
}


module_init(hello_init);
module_exit(hello_exit);



MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");

