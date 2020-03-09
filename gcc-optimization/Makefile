obj-m += test.o
test-y += main.o func.o

EXTRA_CFLAGS += -O3

#SUBARCH := $(shell uname -m | sed -e s/i.86/i386/)
#ARCH ?= $(SUBARCH)
ARCH := arm

#CROSS_COMPILE ?=
#CROSS_COMPILE := /home/neo/Android/1319/trunk-10/phoenix/toolchain/asdk64-4.9.4-a53-EL-3.10-g2.19-a64nt-160307/bin/asdk64-linux-
CROSS_COMPILE := /home/neo/Android/1319/trunk-10/phoenix/toolchain/asdk-6.4.1-a53-EL-4.9-g2.26-a32nut-180831/bin/arm-linux-

#KVER  := $(shell uname -r)
#KSRC := /lib/modules/$(KVER)/build
KSRC := /home/neo/Android/1319/trunk-10/linux-kernel

all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(PWD) modules

objdump:
	$(CROSS_COMPILE)objdump -S func.o

clean:
	$(MAKE) -C $(KSRC) M=$(PWD) clean


