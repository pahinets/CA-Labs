#!/bin/bash
# Run actual module commands in a visible terminal for evidence.
set -u
cd /home/pahinets/CA-Labs/lab5 || exit 1
export LC_ALL=C
run() {
    printf '$'; printf ' %s' "$@"; printf '\n'
    "$@"
    printf '[exit %s]\n' "$?"
}
mode=${1:-3}
printf 'LAB 5 | Ubuntu 24.04 | WSL2 | %s\n' "$(uname -r)"
if [ "$mode" = metadata ]; then
    run modinfo ./hello1.ko
    run modinfo ./hello2.ko
    exit
fi
if [ "$mode" = check ]; then
    run make check KDIR=/home/pahinets/.cache/ak5-kernel
    exit
fi
if [ -d /sys/module/hello1 ] || [ -d /sys/module/hello2 ]; then
    printf 'Refusing to touch preloaded hello modules.\n'
    exit 1
fi
before=$(dmesg --raw | wc -l)
if [ "$mode" = dependency ]; then
    run insmod ./hello2.ko
    run insmod ./hello1.ko
    run insmod ./hello2.ko howmany=3
    printf '\n$ lsmod | grep -E "^hello[12]"\n'
    lsmod | grep -E '^hello[12]'
    run rmmod hello1
    run rmmod hello2
    run rmmod hello1
else
    run insmod ./hello1.ko
    if [ "$mode" = default ]; then
        run insmod ./hello2.ko
    else
        run insmod ./hello2.ko "howmany=$mode"
    fi
    if [ -d /sys/module/hello2 ]; then
        run cat /sys/module/hello2/parameters/howmany
        run rmmod hello2
    fi
    run rmmod hello1
fi
printf '\n$ dmesg --raw | tail -n +%s | grep -E "hello[12]"\n' "$((before + 1))"
dmesg --raw | tail -n +"$((before + 1))" | grep -E 'hello[12]'
