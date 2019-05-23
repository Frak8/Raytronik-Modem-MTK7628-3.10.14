#!/bin/sh
if [ -f /bin/factory-partition.bin ]
then 
	rm /bin/factory-partition.bin
fi

dd if=/dev/mtd3 of=/bin/factory-partition.bin
chmod +x /bin/mac_set
mac_set	$1
sleep 2
mtd write /bin/factory-partition.bin /dev/mtd3
sleep 3
echo y | firstboot 

reboot
