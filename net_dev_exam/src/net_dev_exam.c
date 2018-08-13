#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/netdevice.h>
#include <linux/if_ether.h>



static int __init hello_init(void)
{
	struct net_device *netdev;
	u8 macaddr[ETH_ALEN];

	pr_info("%s: init\n", __func__);

	netdev = __dev_get_by_name(&init_net, "wlan1");
	memcpy(macaddr, netdev->dev_addr, ETH_ALEN);
	print_hex_dump(KERN_INFO, "mac addr: ", DUMP_PREFIX_OFFSET, 16, 1,
		       macaddr, ETH_ALEN, true);

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
