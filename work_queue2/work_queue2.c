#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>	/* for sleep */
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>		/* for kmalloc */
#include <linux/workqueue.h>


struct workqueue_struct *wq1, *wq2;
struct work_struct work1;
struct work_struct work2;

static void work_handler1(struct work_struct *work)
{
	int i;

	pr_info("%s: pid=%d\n", __func__, current->pid);

	for (i=0; i<30; i++) {
		pr_info("%s: i=%d, cpu=%d\n", __func__, i, smp_processor_id());
		msleep(100);
	}
}

static void work_handler2(struct work_struct *work)
{
	int i;

	pr_info("%s: pid=%d\n", __func__, current->pid);

	for (i=0; i<3; i++) {
		pr_info("%s: i=%d, cpu=%d\n", __func__, i, smp_processor_id());
		msleep(1000);
	}
}

static int __init hello_init(void)
{
	pr_info("%s: pid=%d\n", __func__, current->pid);
	wq1 = create_singlethread_workqueue("for work queue 1");
	wq2 = create_singlethread_workqueue("for work queue 2");

	INIT_WORK(&work1, work_handler1);
	INIT_WORK(&work2, work_handler2);
	queue_work(wq1, &work1);
	queue_work(wq2, &work2);
	return 0;
}

static void __exit hello_exit(void)
{
	cancel_work_sync(&work1);
	cancel_work_sync(&work2);
	destroy_workqueue(wq1);
	destroy_workqueue(wq2);
	pr_info("%s: bye\n", __func__);
}


module_init(hello_init);
module_exit(hello_exit);



MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");

