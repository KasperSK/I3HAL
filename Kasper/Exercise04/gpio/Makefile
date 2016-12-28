#FILE: Makefile
ifeq ($(KERNELRELEASE),)
#If we are not invoked form the
#kernel build system (KERNELRELEASE = "")
KERNELDIR = ~/sources/linux-3.2.6/
PWD := $(shell pwd)

default:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-angstrom-linux-gnueabi- -C $(KERNELDIR) M=$(PWD) modules

else
# called from kernel build system:
# just declare what our modules are.
	obj-m := gpio.o
endif

clean:
	rm -rf *.o *.ko *.mod.c *.order *.symvers .*.cmd .tmp_versions

.PHONY: modules clean
