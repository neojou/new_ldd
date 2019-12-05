#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/timex.h>


static void func(void)
{
	int i, val;

	for (i = 0; i < 10; i++)
		val *= 2;
}

static unsigned long loop_count	= 1000;

static DEFINE_MUTEX(kb_lock);

static ssize_t loop_count_show(struct kobject *kobj,
			       struct kobj_attribute *attr,
			       char *buf)
{
	ssize_t res;
	mutex_lock(&kb_lock);
	res = scnprintf(buf, PAGE_SIZE, "%lu\n", loop_count);
	mutex_unlock(&kb_lock);
	return res;
}

static ssize_t loop_count_store(struct kobject *kobj,
				struct kobj_attribute *attr,
				const char *buf,
				size_t count)
{
	unsigned long val;
	int err = kstrtoul(buf, 0, &val);
	if (err < 0)
		return err;
	if (val < 1)
		return -EINVAL;
	mutex_lock(&kb_lock);
	loop_count = val;
	mutex_unlock(&kb_lock);
	return count;
}

static int do_bench_by_ktime(char *buf)
{
	unsigned long long t1, t2;
	unsigned long long total = 0;
	int i;


	for (i = 0; i < loop_count; i++) {
		t1 = ktime_get();	
		func();
		t2 = ktime_get();

		total += ktime_to_ns(ktime_sub(t2, t1));
	}

	return scnprintf(buf, PAGE_SIZE, "avg: %lld ns\n", total / loop_count);
}

static int do_bench_by_cycle(char *buf)
{
	unsigned long long t1, t2;
	unsigned long long total = 0;
	int i;


	for (i = 0; i < loop_count; i++) {
		t1 = get_cycles();	
		func();
		t2 = get_cycles();

		total += t2 - t1;
	}

	return scnprintf(buf, PAGE_SIZE, "avg: %lld cycles\n", total / loop_count);
}

static ssize_t cycles_show(struct kobject *kobj,
			struct kobj_attribute *attr,
			char *buf)
{
	return do_bench_by_cycle(buf);
}

static ssize_t ktime_show(struct kobject *kobj,
			struct kobj_attribute *attr,
			char *buf)
{
	return do_bench_by_ktime(buf);
}

static struct kobj_attribute loop_count_attr = __ATTR_RW(loop_count);
static struct kobj_attribute cycles_attr = __ATTR_RO(cycles);
static struct kobj_attribute ktime_attr = __ATTR_RO(ktime);
static struct attribute *bench_attributes[] = {
	&loop_count_attr.attr,
	&cycles_attr.attr,
	&ktime_attr.attr,
	NULL
};
static struct attribute_group bench_attr_group = {
	.attrs = bench_attributes,
};
static struct kobject *bench_kobj;


static int __init hello_init(void)
{
	int rc;

	bench_kobj = kobject_create_and_add("kbench", kernel_kobj);
	if (!bench_kobj)
		return -ENOMEM;

	rc = sysfs_create_group(bench_kobj, &bench_attr_group);
	if (rc) {
		kobject_put(bench_kobj);
		return rc;
	}

	return 0;
}

static void __exit hello_exit(void)
{
	kobject_put(bench_kobj);
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");

