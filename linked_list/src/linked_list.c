#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>

struct pet {
	char *name;
	struct list_head list;
};

static LIST_HEAD(petlist);

static int __init hello_init(void)
{
	struct pet dog1, dog2;
	struct pet *mpet;

	dog1.name = "Snow";
	dog2.name = "Dragon";

	INIT_LIST_HEAD(&(dog1.list));
	INIT_LIST_HEAD(&(dog2.list));

	list_add_tail(&(dog1.list), &petlist);
	list_add_tail(&(dog2.list), &petlist);

	list_for_each_entry(mpet, &petlist, list) {
		pr_info("my pet name: %s\n", mpet->name);
	}

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
