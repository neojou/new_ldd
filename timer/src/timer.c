#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>

static struct timer_list my_timer;

static void my_timer_callback(struct timer_list *t)
{
	pr_info("%s is called\n", __func__);
}

static int __init hello_init(void)
{
	int ret;
	timer_setup(&my_timer, my_timer_callback, 0);

	pr_info("%s: fire timer after 500ms\n", __func__);
	ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(500));
	if (ret)
		pr_err("%s: timer fired failed, ret=%d\n", __func__, ret);

	return 0;
}

static void __exit hello_exit(void)
{
	int ret;

	ret = del_timer(&my_timer);
	if (ret)
		pr_err("%s: timer is in use, cannot delete, ret=%d\n",
		       __func__, ret);
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");
