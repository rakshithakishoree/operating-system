#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/list.h>

struct jiffies_entry {
	unsigned long long jiffies_val;
	unsigned long timestamp;
	struct list_head list;
};

static LIST_HEAD(jiffies_list);

static int jiffies_proc_show(struct seq_file *m, void *v)
{
	struct jiffies_entry *entry;

	entry = kmalloc(sizeof(struct jiffies_entry), GFP_KERNEL);
	if (!entry) {
		pr_err("Failed to allocate memory for jiffies_entry\n");
		return -ENOMEM;
	}

	entry->jiffies_val = get_jiffies_64();
	entry->timestamp = jiffies_to_msecs(jiffies);
	INIT_LIST_HEAD(&entry->list);

	list_add_tail(&entry->list, &jiffies_list);

	seq_printf(m, "Jiffies = %llu, Timestamp = %lu\n", entry->jiffies_val, entry->timestamp);

	return 0;
}

static int __init proc_jiffies_init(void)
{
	proc_create_single("jiffies", 0, NULL, jiffies_proc_show);
	return 0;
}

static void __exit proc_jiffies_cleanup(void)
{
	struct jiffies_entry *entry, *tmp;

	list_for_each_entry_safe(entry, tmp, &jiffies_list, list) {
		list_del(&entry->list);
		kfree(entry);
	}

	remove_proc_entry("jiffies", NULL);
}

module_init(proc_jiffies_init);
module_exit(proc_jiffies_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rak");
MODULE_DESCRIPTION("A simple LKM with linked list");

