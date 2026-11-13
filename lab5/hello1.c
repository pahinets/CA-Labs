// SPDX-License-Identifier: Dual BSD/GPL
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>

#include "hello1.h"

MODULE_AUTHOR("Tovstopiat O.");
MODULE_DESCRIPTION("Lab 5: exported greeting and print duration list");
MODULE_LICENSE("Dual BSD/GPL");

struct hello_event {
	struct list_head node;
	ktime_t start;
	ktime_t end;
};

static LIST_HEAD(hello_list);

int print_hello(void)
{
	struct hello_event *event;

	event = kmalloc(sizeof(*event), GFP_KERNEL);
	if (!event)
		return -ENOMEM;

	event->start = ktime_get();
	pr_info("hello1: Hello, world!\n");
	event->end = ktime_get();
	list_add_tail(&event->node, &hello_list);
	return 0;
}
EXPORT_SYMBOL(print_hello);

static int __init hello1_init(void)
{
	pr_info("hello1: loaded\n");
	return 0;
}

static void __exit hello1_exit(void)
{
	struct hello_event *event, *next;
	unsigned int index = 0;

	list_for_each_entry_safe(event, next, &hello_list, node) {
		pr_info("hello1: print %u took %lld ns\n", ++index,
			ktime_to_ns(ktime_sub(event->end, event->start)));
		list_del(&event->node);
		kfree(event);
	}
	pr_info("hello1: unloaded, freed %u events\n", index);
}

module_init(hello1_init);
module_exit(hello1_exit);
