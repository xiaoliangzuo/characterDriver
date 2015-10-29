#! /bin/bash
rm -rf /dev/globaldev1
rm -rf /dev/globaldev2
rm -rf /dev/globaldev3
rm -rf /dev/globaldev4
rmmod globalmem.ko
insmod globalmem.ko
mknod /dev/globaldev1 c 250 0
mknod /dev/globaldev2 c 250 1
mknod /dev/globaldev3 c 250 2
mknod /dev/globaldev4 c 250 3
gcc -o re read.c -Wall
gcc -o wr write.c -Wall
