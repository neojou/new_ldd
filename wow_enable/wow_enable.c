#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>

static int write_to_file(const char *path, u8 *buf, u32 len)
{
	struct file *fp;
	int sum = 0, wlen = 0;

	fp = filp_open(path, O_CREAT | O_WRONLY, 0666);
	if (IS_ERR(fp)) {
		pr_err("file open error: %s\n", path);
		return 0;
	}

	if (!(fp->f_mode & FMODE_CAN_WRITE))
		return -EPERM;

	while (sum < len) {
		wlen = kernel_write(fp, buf + sum, len - sum, &fp->f_pos);
		if (wlen > 0)
			sum += wlen;
		else
			break; 		
	}

	return 0;
}


static int __init hello_init(void)
{
	pr_info("write 0 to proc wow_enable");
	write_to_file("/proc/net/rtl88x2cu/wlan1/wow_enable", "0", 1);
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("write 1 to proc wow_enable");
	write_to_file("/proc/net/rtl88x2cu/wlan1/wow_enable", "1", 1);
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");

