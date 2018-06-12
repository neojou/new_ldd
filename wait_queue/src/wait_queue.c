#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/workqueue.h>


static DECLARE_WAIT_QUEUE_HEAD(mwq);


static struct work_struct wrk;
static bool wake_up = false;

static void work_handler(struct work_struct *work)
{
	pr_info("%s start sleep 3 sec.\n", __func__);
	msleep(3000);
	wake_up = true;
	pr_info("%s before send wake_up\n", __func__);
	wake_up_interruptible(&mwq);
	pr_info("%s after send wake_up\n", __func__);
}

static int __init hello_init(void)
{
	INIT_WORK(&wrk, work_handler);
	schedule_work(&wrk);

	pr_info("%s: Start to wait\n", __func__);
	wait_event_interruptible(mwq, wake_up);
	pr_info("%s: wakeup\n", __func__);

	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("How're you doing\n");
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");
