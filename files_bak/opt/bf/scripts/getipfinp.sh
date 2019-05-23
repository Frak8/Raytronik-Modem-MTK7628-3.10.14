#!/bin/sh

INF=$1
found=""
while read -r line; do
	if [ "XXX$found" = "XXX" ]; then
		inf=`echo $line | cut -f 1 -d' '`
		if [ "XXX$inf" = "XXX$INF" ]; then
			found="true"
		fi
	else
		addrline=`echo $line | grep "inet addr"`
		addr=`echo $line | cut -f 2 -d':' | cut -f 1 -d' '`
		echo $addr
		exit
	fi
done
