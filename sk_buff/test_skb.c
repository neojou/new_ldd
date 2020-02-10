#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>



/* module */

static int __init hello_init(void)
{
	pr_info("%s: \n", __func__);

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

