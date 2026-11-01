# Lab 4: Linux kernel module (hello)

Module with a `howmany` (uint) parameter that prints "Hello, world!"
that many times and keeps a list of timestamps (`ktime_get()`),
printed and freed in `hello_exit()`.

## Build (ARM, kernel from LR3)

    source ~/repos/env-eabi.sh
    export KDIR=$HOME/repos/linux-stable
    make
    make check

## Run (QEMU / BBB)

    insmod hello.ko howmany=3
    rmmod hello

`howmany`: default 1; 0 or 5..10 prints a warning; more than 10 returns -EINVAL.
