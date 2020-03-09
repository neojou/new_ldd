#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

u16 chksum1(u16 *data, int len)
{
	int i;
	u16 chksum = 0;

	for (i = 0; i < len; i++)
		chksum ^= *(data + 2 * i) ^ *(data + 2 * i + 1);

	return chksum;
}

u16 chksum2(u16 *data, int len)
{
	u16 chksum = 0;

	while (len--)
		chksum ^= *data++;

	return chksum;
}

