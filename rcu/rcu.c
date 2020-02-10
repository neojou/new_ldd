#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>	/* for sleep */
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>		/* for kmalloc */
#include <linux/workqueue.h>
#include <linux/list.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>


struct mylist {
	int id;
	struct list_head list;
};

struct mylist head;


static DEFINE_SPINLOCK(my_lock);


static void mylist_init(struct mylist *ml)
{
	INIT_LIST_HEAD(&(ml->list));
}

/*
static void mylist_show(struct mylist *ml)
{
	struct list_head *p;
	struct mylist *entry;

	list_for_each(p, &(ml->list)) {
		entry = list_entry(p, struct mylist, list);
		pr_info("id=%d\n", entry->id);
	}
}
*/

static void mylist_add(struct mylist *ml, int id)
{
	struct mylist *new_data;

	new_data = kmalloc(sizeof(struct mylist), GFP_KERNEL);
	if (!new_data) {
		pr_err("%s: no memory\n", __func__);
		return;
	}
		
	new_data->id = id;
	list_add_tail(&new_data->list, &ml->list);
}

static void mylist_deinit(struct mylist *ml)
{
	struct list_head *p;
	struct mylist *entry;

	list_for_each(p, &(ml->list)) {
		entry = list_entry(p, struct mylist, list);
		kfree(entry);
	}
}

static bool mylist_has_id(struct mylist *ml, int id)
{
	struct list_head *p;
	struct mylist *entry;

	list_for_each(p, &(ml->list)) {
		entry = list_entry(p, struct mylist, list);
		if (entry->id == id)
			return true;
	}

	return false;
}

static bool mylist_has_id_and_del(struct mylist *ml, int id)
{
	struct list_head *p;
	struct mylist *entry;

	list_for_each(p, &(ml->list)) {
		entry = list_entry(p, struct mylist, list);
		if (entry->id == id) {
			list_del(&entry->list);
			kfree(entry);
			return true;
		}
	}

	return false;
}

static bool mylist_has_id_and_del_lock(struct mylist *ml, int id)
{
	struct list_head *p;
	struct mylist *entry;

	spin_lock(&my_lock);
	list_for_each(p, &ml->list) {
		entry = list_entry(p, struct mylist, list);
		if (entry->id == id) {
			list_del(&entry->list);
			kfree(entry);
			spin_unlock(&my_lock);
			return true;
		}
	}
	spin_unlock(&my_lock);

	return false;
}

static bool mylist_has_all_id(struct mylist *ml, int max)
{
	int i;

	for (i=1; i<=max; i++) 
		if (!mylist_has_id(ml, i)) 
			return false;

	return true;
}

static bool mylist_has_all_id_and_del(struct mylist *ml, int max)
{
	int i;

	for (i=1; i<=max; i++) 
		if (!mylist_has_id_and_del(ml, i)) 
			return false;

	return true;
}

/* perf1 */

static int debugfs_perf1(struct seq_file *s, void *data)
{
	int i;
	bool is_correct;
	u64 start_time, delta;

	mylist_init(&head);

	for (i=1; i<=10000; i++) 
		mylist_add(&head, i);

	start_time = ktime_get_ns();
	is_correct = mylist_has_all_id(&head, 10000);
	delta = ktime_get_ns() - start_time;

	seq_printf(s, "mylist has all 1~10000: %d\n", is_correct);
	seq_printf(s, "spend time: %llu ns\n", delta);

	mylist_deinit(&head);

	return 0;
}

static int debugfs_perf1_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, debugfs_perf1, inode->i_private);
}

static const struct file_operations file_ops_perf1 = {
	.owner = THIS_MODULE,
	.open = debugfs_perf1_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

/* perf2 */

static int debugfs_perf2(struct seq_file *s, void *data)
{
	int i;
	bool is_correct;
	u64 start_time, delta;

	mylist_init(&head);

	for (i=1; i<=10000; i++) 
		mylist_add(&head, i);

	start_time = ktime_get_ns();
	is_correct = mylist_has_all_id_and_del(&head, 10000);
	delta = ktime_get_ns() - start_time;

	seq_printf(s, "mylist has all 1~10000: %d\n", is_correct);
	seq_printf(s, "spend time: %llu ns\n", delta);

	mylist_deinit(&head);

	return 0;
}

static int debugfs_perf2_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, debugfs_perf2, inode->i_private);
}

static const struct file_operations file_ops_perf2 = {
	.owner = THIS_MODULE,
	.open = debugfs_perf2_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

/* perf 3 */

struct workqueue_struct *wq1, *wq2, *wq3, *wq4;
struct work_struct work1, work2, work3, work4;
static bool is_correct;

static void work_handler1(struct work_struct *work)
{
	int i;

	for (i=1; i<=2500; i++) {
		if (!mylist_has_id_and_del_lock(&head, i)) {
			is_correct = false;
			break;
		}
	}
}

static void work_handler2(struct work_struct *work)
{
	int i;

	for (i=2501; i<=5000; i++) {
		if (!mylist_has_id_and_del_lock(&head, i)) {
			is_correct = false;
			break;
		}
	}
}

static void work_handler3(struct work_struct *work)
{
	int i;

	for (i=5001; i<=7500; i++) {
		if (!mylist_has_id_and_del_lock(&head, i)) {
			is_correct = false;
			break;
		}
	}
}

static void work_handler4(struct work_struct *work)
{
	int i;

	for (i=7501; i<=10000; i++) {
		if (!mylist_has_id_and_del_lock(&head, i)) {
			is_correct = false;
			break;
		}
	}
}

static int debugfs_perf3(struct seq_file *s, void *data)
{
	int i;
	u64 start_time, delta;

	mylist_init(&head);

	for (i=1; i<=10000; i++) 
		mylist_add(&head, i);

	is_correct = true;

	wq1 = create_singlethread_workqueue("for work queue 1");
	wq2 = create_singlethread_workqueue("for work queue 2");
	wq3 = create_singlethread_workqueue("for work queue 3");
	wq4 = create_singlethread_workqueue("for work queue 4");

	INIT_WORK(&work1, work_handler1);
	INIT_WORK(&work2, work_handler2);
	INIT_WORK(&work3, work_handler3);
	INIT_WORK(&work4, work_handler4);

	start_time = ktime_get_ns();

	queue_work(wq1, &work1);
	queue_work(wq2, &work2);
	queue_work(wq3, &work3);
	queue_work(wq4, &work4);

	flush_workqueue(wq1);
	flush_workqueue(wq2);
	flush_workqueue(wq3);
	flush_workqueue(wq4);

	delta = ktime_get_ns() - start_time;

	seq_printf(s, "mylist has all 1~10000: %d\n", is_correct);
	seq_printf(s, "Thread spend time: %llu ns\n", delta);

	mylist_deinit(&head);

	return 0;
}

static int debugfs_perf3_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, debugfs_perf3, inode->i_private);
}

static const struct file_operations file_ops_perf3 = {
	.owner = THIS_MODULE,
	.open = debugfs_perf3_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

/* debugfs */

static struct dentry *debugfs_topdir;
static void debugfs_init(void)
{

	debugfs_topdir = debugfs_create_dir("rcu", NULL);

	if (!debugfs_create_file("perf1", S_IFREG| S_IRUGO, 
				 debugfs_topdir, NULL,
				 &file_ops_perf1))		
		pr_err("Unable to initialize rcu/perf1\n");

	if (!debugfs_create_file("perf2", S_IFREG| S_IRUGO, 
				 debugfs_topdir, NULL,
				 &file_ops_perf2))		
		pr_err("Unable to initialize rcu/perf2\n");

	if (!debugfs_create_file("perf3", S_IFREG| S_IRUGO, 
				 debugfs_topdir, NULL,
				 &file_ops_perf3))		
		pr_err("Unable to initialize rcu/perf3\n");
}

void debugfs_deinit(void)
{
	if (debugfs_topdir)
		debugfs_remove_recursive(debugfs_topdir);
}

/* module */

static int __init hello_init(void)
{
	pr_info("%s: pid=%d\n", __func__, current->pid);

	debugfs_init();
#if 0
#endif

	return 0;
}

static void __exit hello_exit(void)
{
#if 0
	cancel_work_sync(&work1);
	cancel_work_sync(&work2);
	destroy_workqueue(wq1);
	destroy_workqueue(wq2);
#endif
	debugfs_deinit();
	pr_info("%s: bye\n", __func__);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");

