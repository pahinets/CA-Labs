// SPDX-License-Identifier: Dual BSD/GPL
/*
 * Lab 4: hello module with a "howmany" parameter and a list of timestamps.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Tovstopiat O. <tovstopiat.oleksandr_io-42@edu.kpi.ua>");
MODULE_DESCRIPTION("Lab 4: Hello, world! printed howmany times with timestamps");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int howmany = 1;
module_param(howmany, uint, 0444);
MODULE_PARM_DESC(howmany, "How many times to print \"Hello, world!\" (default 1, max 10)");

struct hello_event {
	struct list_head node;
	ktime_t time;
};

static LIST_HEAD(hello_list);

static void hello_free_list(void)
{
	struct hello_event *ev, *tmp;

	list_for_each_entry_safe(ev, tmp, &hello_list, node) {
		list_del(&ev->node);
		kfree(ev);
	}
}

static int __init hello_init(void)
{
	struct hello_event *ev;
	unsigned int i;

	if (howmany == 0 || (howmany >= 5 && howmany <= 10))
		pr_warn("hello: howmany = %u, continuing anyway\n", howmany);

	if (howmany > 10) {
		pr_err("hello: howmany = %u is greater than 10, refusing to load\n",
		       howmany);
		return -EINVAL;
	}

	for (i = 0; i < howmany; i++) {
		ev = kmalloc(sizeof(*ev), GFP_KERNEL);
		if (!ev) {
			hello_free_list();
			return -ENOMEM;
		}
		ev->time = ktime_get();
		list_add_tail(&ev->node, &hello_list);
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_event *ev, *tmp;

	list_for_each_entry_safe(ev, tmp, &hello_list, node) {
		pr_info("hello: event time = %lld ns\n", ktime_to_ns(ev->time));
		list_del(&ev->node);
		kfree(ev);
	}
}

module_init(hello_init);
module_exit(hello_exit);
