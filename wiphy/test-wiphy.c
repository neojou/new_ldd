#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/cfg80211.h>

static struct cfg80211_ops my_cfg80211_ops = {
};



static int __init hello_init(void)
{
	int ret = 0;
	struct wiphy *wiphy;

	wiphy = wiphy_new(&my_cfg80211_ops, 0);
	if (!wiphy) {
		pr_err("wiphy_new() failed\n");
		return -ENOMEM;
	}

	ret = wiphy_register(wiphy);
	if (ret < 0) {
		pr_err("wiphy_register() failed, ret=%d\n", ret);
	}

	return ret;
}

static void __exit hello_exit(void)
{
	pr_info("How're you doing\n");
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");
