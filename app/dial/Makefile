.PHONY:all install clean

BINDIR=./bin
OBJDIR=./obj
SRCDIR=./src
CROSS_COMPILE=/yanfa/heqiang/beijing-jianyou/new/openwrt-3.10.14/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-
export STAGING_DIR=/yanfa/heqiang/beijing-jianyou/new/openwrt-3.10.14/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/
CC:=$(CROSS_COMPILE)gcc

SRCLIST=$(wildcard $(SRCDIR)/*.c)
OBJLIST=$(SRCLIST:%.c=%.o)

TARGET=dial

all:$(OBJLIST) $(TARGET) install

$(OBJLIST):%.o:%.c 
	$(CC) -c $< -o $@

$(TARGET):$(OBJLIST) 
	$(CC) -o $(TARGET) $(OBJLIST)

install:
	[ -d $(BINDIR) ]|| mkdir bin 
	[ -d $(OBJDIR) ]|| mkdir obj
	mv $(OBJLIST) $(OBJDIR);
	mv $(TARGET) $(BINDIR);

clean:
	rm $(OBJDIR)/* $(BINDIR)/*


.DELETE_ON_ERROR: 
