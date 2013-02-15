#!/bin/sh
qemu-system-x86_64 -m 128 -fda ./image/floppy.img -boot a -smp 2
