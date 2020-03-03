obj-m += gcc_extension.o
KVERSION := $(shell uname -r)
CONFIG_KASAN = y
all:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) clean


