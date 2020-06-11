#
target := sipfw
$(target)-objs := sipfw_k.o sipfw_k_common.o sipfw_k_file.o
obj-m := $(target).o
KERNELDIR = /lib/modules/$(shell uname -r)/build

.PHONY: all install uninstall clean

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install:
	insmod $(target).ko

uninstall:
	rmmod $(target).ko

clean:
	rm -rf *.o *.mod .*.cmd *.mod.c *.ko *.order
	rm -rf Module.symvers *.cmd .tmp_versions
	rm -rf test

test:test.c
	gcc -o $@ $^
