obj-m += atomic1.o
obj-m += atomic2.o
obj-m += atomic3.o
obj-m += atomic4.o
KVERSION := $(shell uname -r)
all:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
