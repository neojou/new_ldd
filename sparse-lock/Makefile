obj-m += sparse-lock.o
KVERSION := $(shell uname -r)
all:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) C=2 modules

clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
