#!/bin/sh

#服务器资源路径
wwwDoc="\/mnt\/bf\/res"

#服务器地址
confDir="/etc"
serverDir="/etc"

######替换服务器资源路径######


######替换路由器唯一标示######
m1=`/usr/sbin/iwpriv ra0 e2p 80 |sed -n 2p |awk -F x '{print $3}'`
m2=`/usr/sbin/iwpriv ra0 e2p 82 |sed -n 2p |awk -F x '{print $3}'`
m3=`/usr/sbin/iwpriv ra0 e2p 84 |sed -n 2p |awk -F x '{print $3}'`
Idintity=`echo ${m1}${m2}${m3}| sed 's/ //g'`
if [ -z Idintity ]
then
Idintity=`cat /sys/class/net/ra0/address`
fi
if [ -n Idintity ]  
then
sed -i "s/^.*server_mac.*$/server_mac $Idintity;/" $serverDir/nginx/nginx.conf

sed -i "s/^.*add_header.*$/add_header mac $Idintity;/" $serverDir/nginx/nginx.conf

#sed -i "s/^.*root.*$/root $wwwDoc;/g" /etc/nginx/nginx.conf
fi

rm -rf /META-INF 2> /dev/null
mkdir /META-INF
PRE_FIX="channel_C"
CHANNEL="/META-INF/"$PRE_FIX$Idintity
touch $CHANNEL

wwwDoc="/mnt/bf/res"
DIR="$wwwDoc/welcome/app"

APK_NUM=`ls -1 $DIR | grep '.*\.apk$' | wc -l`

Log_File="/root/log.txt"
if [ $APK_NUM -ne 1 ]; then
	echo "apk not exist" >> $Log_File
	exit 2
fi

FIND_APK=`ls -1 $DIR | grep '.*\.apk$'`
cd $DIR
zip $FIND_APK $CHANNEL
if [ $? -ne 0 ]; then
	echo "zip error" >> $Log_File
	exit 2
fi
