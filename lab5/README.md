# Lab 5 — two dependent Linux kernel modules

This is the assignment headed **Лабораторна робота 5** inside the supplied
`#4 лабораторна робота 4_2_2025.pdf`. It extends the existing `../lab4` module.

## Files

- `hello1.c`: exports `int print_hello(void)`; owns the timestamp list and frees it on unload.
- `hello2.c`: read-only `uint howmany` parameter (default 1); calls the exported function.
- `inc/hello1.h`: shared declaration, included by name through `ccflags-y`.
- `Makefile`: kbuild targets for both modules; `check` and `test` targets.
- `test_modules.py`: integration tests using the actual kernel, not mocks.
- `demo.sh`: real terminal commands used for the screenshots.
- `results/`: actual build, checkpatch, environment and integration logs.

## Build on the configured WSL Ubuntu 24.04

```sh
cd /home/pahinets/CA-Labs/lab5
export KDIR=/home/pahinets/.cache/ak5-kernel
make KDIR="$KDIR" CC=gcc-13
make check KDIR="$KDIR"
```

For another Linux installation, use a complete prepared build tree matching
its running kernel (`KDIR=/lib/modules/$(uname -r)/build` by default).

The WSL kernel headers initially present in `/usr/src` were incomplete.
A separate source tree was prepared in `/home/pahinets/.cache/ak5-kernel`
from the existing WSL kernel checkout, using `/proc/config.gz`, GCC 13 and
`LOCALVERSION=`. `Module.symvers` was copied from the existing matching
kernel build. The old checkout, existing lab4 and WSL configuration were
not changed. Both modules were successfully loaded without forced version
or symbol checks into `6.18.58.1-microsoft-standard-WSL2`.
BTF generation was skipped by kbuild because the isolated tree has no
`vmlinux`; this does not prevent loading the modules.

## Load and inspect (root required)

```sh
insmod ./hello1.ko
insmod ./hello2.ko howmany=7
cat /sys/module/hello2/parameters/howmany
modinfo ./hello1.ko
modinfo ./hello2.ko
rmmod hello2
rmmod hello1
dmesg | grep -E 'hello[12]'
```

`howmany=0` and values 5–10 produce warnings but are accepted.
Values 1–4 are accepted without warnings. Values above 10 return `-EINVAL`.
The printed durations in nanoseconds measure the interval around `pr_info`,
not the full time until a physical display renders the message.

`insmod hello2.ko` without hello1 fails with `Unknown symbol print_hello`.
`insmod` does not automatically load dependencies. `modinfo hello2.ko`
reports `depends: hello1`. Removing hello1 while hello2 is loaded is refused;
remove hello2 first. List entries survive unloading hello2 and are released
only when hello1 is unloaded.

## Run the verified integration tests

In WSL as root:

```sh
cd /home/pahinets/CA-Labs/lab5
python3 test_modules.py
```

From Windows:

```sh
wsl.exe -d Ubuntu-24.04 -u root --cd /home/pahinets/CA-Labs/lab5 --exec python3 test_modules.py
```

The suite refuses to touch hello modules already loaded before its run.
The verified run passed 5 test methods: default parameter; all explicit
values 0–11 and 15; missing provider and dependency-protected unload;
accumulation across client reloads; metadata, vermagic and imported/exported
symbol checks. Allocator failure injection and arbitrary concurrent clients
were not tested; the lab client invokes the function sequentially.

## Repository and report

The laboratory is maintained in branch
[`feat/lab5-kernel-modules`](https://github.com/pahinets/CA-Labs/tree/feat/lab5-kernel-modules/lab5).
The final Ukrainian DOCX and PDF reports are in `reports/`;
`screenshots/` contains the seven original terminal captures and `results/`
contains the recorded build, checkpatch and integration evidence.
Generated `.ko` and other kbuild artifacts are intentionally not tracked;
rebuild them for the kernel where they will be loaded.
