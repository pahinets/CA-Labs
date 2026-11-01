# Usage:
#   export KDIR=$HOME/repos/linux-stable
#   make                          # native / QEMU x86
#   make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-   # BeagleBone Black

ifneq ($(KERNELRELEASE),)
obj-m := hello.o
else
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f hello.c
endif
