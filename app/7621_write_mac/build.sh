#!/bin/sh

CROSS_COMPILE=/yanfa/zhu/7621/mtk-openwrt-3.10.14/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/
export STAGING_DIR=/yanfa/zhu/7621/mtk-openwrt-3.10.14/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/
CC=${CROSS_COMPILE}mipsel-openwrt-linux-gcc

${CC} -o mac_set mac-set.c
