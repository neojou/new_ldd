#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>	/* for sleep */
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>		/* for kmalloc */
#include <linux/workqueue.h>

struct work_data {
	struct work_struct my_work;
	int the_data;
};

static void work_handler(struct work_struct *work)
{
	struct work_data *my_data = container_of(work, struct work_data,
						 my_work);

	pr_info("%s: data is %d\n", __func__, my_data->the_data);
	msleep(2000);
	kfree(my_data);
	pr_info("%s: bye\n", __func__);
}

static int __init hello_init(void)
{
	struct work_data *my_data;

	my_data = kmalloc(sizeof(struct work_data), GFP_KERNEL);
	my_data->the_data = 666;

	INIT_WORK(&my_data->my_work, work_handler);
	schedule_work(&my_data->my_work);
	pr_info("%s: bye\n", __func__);
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("%s: bye\n", __func__);
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");
