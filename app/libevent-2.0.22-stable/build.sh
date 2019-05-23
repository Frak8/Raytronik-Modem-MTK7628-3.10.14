#/bin/sh
export STAGING_DIR=/yanfa/heqiang/MTK/openwrt-2.6.36/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.6-linaro_uClibc-0.9.33.2/
./configure --disable-shared --enable-static --host=mips CC=${STAGING_DIR}bin/mipsel-openwrt-linux-gcc

