CROSS_COMPILE = /opt/linaro-7.5.0/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
CC=$(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip
CURRENT_DIR=$(shell pwd)

INSPUR_COMMON_INC ?= $(CURRENT_DIR)/../../../install/include
INSPUR_INSTALL_BIN ?= $(CURRENT_DIR)/../../../install/bin
INSPUR_INSTALL_LIB ?= $(CURRENT_DIR)/../../../install/lib
INCLUDE_CFLAGS=-I$(INSPUR_COMMON_INC)
LIB_LDFLAGS=-L$(INSPUR_INSTALL_LIB) -lrt -lpthread -lubus -lubox -lblobmsg_json -ljson-c

#you can support the slog to syslogd. you can support the slog to send remote by udp.
#ATOS_INCLUDE_CFLAGS+= -DSUPPORT_SYSLOG -DSUPPORT_SENT2REMOTE

ifneq ($(TC_CFLAGS), )
CFLAGS+=$(TC_CFLAGS)
endif

TARGET_NAME=client

objs = $(patsubst %.c,%.o,$(wildcard *.c))

all:$(TARGET_NAME)

$(TARGET_NAME):$(objs)
	$(CC) $(INCLUDE_CFLAGS)  $(LIB_LDFLAGS) -o $(TARGET_NAME)  $(objs)
	$(STRIP) $(TARGET_NAME)
	@cp -f $(TARGET_NAME) $(INSPUR_INSTALL_BIN)/
	

%.o:%.c
	@echo "compile $@"
	$(CC)  $(CFLAGS) $(INCLUDE_CFLAGS) -c $<

clean:
	@rm -rf $(TARGET_NAME) 
	@rm -rf $(objs)
