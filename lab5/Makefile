ifneq ($(KERNELRELEASE),)
obj-m := hello1.o hello2.o
ccflags-y += -I$(src)/inc
else
KDIR ?= /lib/modules/$(shell uname -r)/build
MODULE_DIR := $(CURDIR)

.PHONY: all clean check test
all:
	$(MAKE) -C $(KDIR) M=$(MODULE_DIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(MODULE_DIR) clean

check:
	perl $(KDIR)/scripts/checkpatch.pl --no-tree -f hello1.c hello2.c inc/hello1.h

test:
	python3 test_modules.py
endif
