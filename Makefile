TARGET:= IPC
SRCDIRS     := . ./src
SOURCES     := $(foreach d,$(SRCDIRS),$(wildcard $(d)/*.c))
OBJECTS     := $(patsubst %.c,build/%.o,$(SOURCES))
DEPS        := $(OBJECTS:.o=.d)

CROSS_COMPILE ?= aarch64-buildroot-linux-gnu-
override CC   := $(CROSS_COMPILE)gcc

SYSROOT := $(shell $(CC) --print-sysroot)

CSTD       	?= gnu11
CPPFLAGS   	+= -Iinclude
CPPFLAGS   	+= -I/home/book/rk3566/IPcamera/ZLMediaKit/api/include
CFLAGS     	+= -std=$(CSTD) -MMD -MP
LDFLAGS	   	+= -L/home/book/rk3566/IPcamera/rk3566-x264/lib
LDFLAGS		+= -L/home/book/rk3566/IPcamera/ZLMediaKit/release/linux/Release
LDFLAGS		+= -L/home/book/rk3566/IPcamera/ZLMediaKit/build/3rdpart/ZLToolKit/lib
LDLIBS		+= -lx264
LDLIBS		+= -lmk_api
LDLIBS		+= -lZLToolKit


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET)

-include $(DEPS)