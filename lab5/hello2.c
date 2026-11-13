// SPDX-License-Identifier: Dual BSD/GPL
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>

#include "hello1.h"

MODULE_AUTHOR("Tovstopiat O.");
MODULE_DESCRIPTION("Lab 5: greeting client with a howmany parameter");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int howmany = 1;
module_param(howmany, uint, 0444);
MODULE_PARM_DESC(howmany, "Number of greetings (default 1, maximum 10)");

static int __init hello2_init(void)
{
	unsigned int i;
	int ret;

	if (howmany > 10) {
		pr_err("hello2: howmany = %u is greater than 10, refusing to load\n",
		       howmany);
		return -EINVAL;
	}
	if (howmany == 0 || (howmany >= 5 && howmany <= 10))
		pr_warn("hello2: howmany = %u, continuing anyway\n", howmany);

	for (i = 0; i < howmany; i++) {
		ret = print_hello();
		if (ret)
			return ret;
	}
	pr_info("hello2: loaded, howmany = %u\n", howmany);
	return 0;
}

static void __exit hello2_exit(void)
{
	pr_info("hello2: unloaded\n");
}

module_init(hello2_init);
module_exit(hello2_exit);
