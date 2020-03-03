#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void)
{
	int a[3];
	char *b;
	int *c;

	pr_info("a %s arrary\n",
		__same_type((a), &(a)[0])?
		"is not" : "is");

	pr_info("b %s arrary\n",
		__same_type((b), &(b)[0])?
		"is not" : "is");
	
	pr_info("b %s char *\n",
		__builtin_types_compatible_p(typeof(b), char *)?
		"is" : "is not");

	pr_info("char *b and int *c are %s\n",
		__same_type(b, c)?
		"the same" : "different");

//	BUILD_BUG_ON_ZERO(__same_type((b), &(b)[0]));
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

