#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

u16 chksum1(u16 *data, int len);
u16 chksum2(u16 *data, int len);

static int __init hello_init(void)
{
	u16 data[] = {0x1234, 0x5678, 0x90AB, 0xCDEF};

	pr_info("chksum1=0x%x\n", chksum1(data, 4/2));
	pr_info("chksum2=0x%x\n", chksum2(data, 4));

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
