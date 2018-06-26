#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kmod.h>


static void work_handler(void)
{
	int ret;

	char *cmd = "/bin/bash";
	char *argv[] = {
		cmd,
		"-c",
		"/bin/ls > /tmp/list",
		NULL,
	};
	char *envp[] = {
		"HOME=/",
		"PATH=/sbin:/bin:/usr/sbin:/usr/bin",
		NULL,
	};

	ret = call_usermodehelper(cmd, argv, envp, UMH_WAIT_PROC);
	pr_info("%s: call_usermodehelper, ret=%d\n", __func__, ret);
}

static int __init hello_init(void)
{
	work_handler();
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
