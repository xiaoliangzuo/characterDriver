#! /bin/bash
rm -rf /dev/globaldev
rmmod globalmem.ko
insmod globalmem.ko
mknod /dev/globaldev c 250 0
gcc test.c
