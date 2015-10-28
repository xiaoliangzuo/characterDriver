obj-m := globalmem.o
kerpath=/usr/src/linux-headers-3.13.0-24-generic
all:
	make -C $(kerpath) M=$(PWD) modules
