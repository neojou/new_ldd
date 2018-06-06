#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


struct dog {
	char *name;
};

struct cat {
	char *name;
};

struct mypets {
	struct dog mydog;
	struct cat mycat;
} mp;


static int __init hello_init(void)
{
	struct mypets *whospets;
	struct cat *mc;

	mp.mydog.name = "SnowDragon";
	mp.mycat.name = "Flower";
	mc = &mp.mycat;
	whospets = container_of(mc, struct mypets, mycat);
	pr_info("my dog's name: %s\n", whospets->mydog.name);
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
