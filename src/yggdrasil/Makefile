OS := $(shell uname)
DISK :="disk.img"
SIZE := 192

PROFILE=False

CFLAGS=-DFUSE_USE_VERSION=26 `pkg-config --cflags python2` `pkg-config --cflags fuse`
LDFLAGS=`pkg-config --libs python2` `pkg-config --libs fuse`

ifeq ($(OS),Linux)
LDFLAGS += -shared
endif

ifeq ($(OS),Darwin)
LDFLAGS += -dynamiclib -Qunused-arguments
endif


all: diskimpl.so yav_dirimpl_fuse.so

prod: bitmap.so inodepack.so waldisk.so xv6inode.so yav_xv6_main.so dirinode.so

.PHONY: verify
verify: diskimpl.so
	python2 verify.py

%.so: %.o
	gcc -march=native -o $@ $< $(LDFLAGS)

%.o: %.c
	gcc -march=native -O2 -c -fPIC $(CFLAGS) $<

%.c: %.pyx
	cython -X profile=$(PROFILE) $<

%.c: %.py
	cythonize -X profile=$(PROFILE) $<

run:
	mkdir -p mnt
	truncate -s $(shell echo 1024*1024*$(SIZE) | bc) $(DISK)
	python yav_xv6_main.py -o max_read=4096 -o max_write=4096 -s mnt -- $(DISK)

.SECONDARY:

.PHONY: clean
clean:
	rm -f *.so *.o *.c *.pyc
