CUR_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

include $(CUR_DIR)/Makefile.inc

LLVM_ARGS := -DLLVM_TARGETS_TO_BUILD=X86 -DCMAKE_BUILD_TYPE=Release

all: readme

readme:
	@cat build-note

build-llvm: get-llvm
	if [ ! -d "$(LLVM_BLD)" ]; then \
		mkdir -p $(LLVM_BLD); \
	fi
	if [ ! -d "$(LLVM_BLD)/bin" ]; then \
		cd $(LLVM_BLD) && cmake $(LLVM_ARGS) $(LLVM_DIR); cd ..; \
		make -C $(LLVM_BLD) -j${NJOB}; \
	fi

build-afl: build-llvm
	make -C $(AFL_DIR) -j${NJOB}

clean-afl:
	make -C $(AFL_DIR) clean

build-afl-llvm-mode: build-afl
	make -C $(AFL_LLVM_DIR) -j${NJOB}

clean-afl-llvm-mode:
	make -C $(AFL_LLVM_DIR) clean

build-syscall: build-afl-llvm-mode
	make -C $(SYSCALL_DIR) -j${NJOB}

clean-syscall:
	make -C $(SYSCALL_DIR) clean

build-ffgcc: build-syscall
	make -C $(FF_GCC_DIR) -j${NJOB}

clean-ffgcc:
	make -C $(FF_GCC_DIR) clean

build-combined: build-ffgcc
	make -C $(AFL_CMB_DIR) -j${NJOB}

build-img-sys: build-combined
	make -C $(AFL_IMG_SYS_DIR) -j${NJOB}

build-lkl-btrfs: get-lkl
	(cd $(LKL_DIR) && ./compile -t btrfs -c && cd -)

build-lkl-ext4: get-lkl
	(cd $(LKL_DIR) && ./compile -t ext4 -c && cd -)

build-lkl-f2fs: get-lkl
	(cd $(LKL_DIR) && ./compile -t f2fs -c && cd -)

build-lkl-xfs: get-lkl
	(cd $(LKL_DIR) && ./compile -t xfs -c && cd -)

get-lkl: build-img-sys
	if [ ! -d "$(LKL_DIR)" ]; then \
		git clone https://github.com/sslab-gatech/lkl.git $(LKL_DIR); \
	fi

get-llvm: 
	if [ ! -d "$(LLVM_DIR)" ]; then \
		wget http://releases.llvm.org/$(LLVM_VERSION)/$(LLVM_SRC).tar.xz && tar xf $(LLVM_SRC).tar.xz && rm $(LLVM_SRC).tar.xz; \
		mv $(LLVM_SRC) $(LLVM_DIR); \
		cd $(LLVM_DIR)/tools && wget http://releases.llvm.org/$(LLVM_VERSION)/$(CLANG_SRC).tar.xz && tar xf $(CLANG_SRC).tar.xz && rm $(CLANG_SRC).tar.xz; \
		mv $(CLANG_SRC) clang; \
		cd ../..; \
	fi

build-btrfs-imgwrp: build-lkl-btrfs
	make -C $(BTRFS_IMGWRP_DIR)

build-ext4-imgwrp: build-lkl-ext4
	make -C $(EXT4_IMGWRP_DIR)

build-f2fs-imgwrp: build-lkl-f2fs
	make -C $(F2FS_IMGWRP_DIR)

build-xfs-imgwrp: build-lkl-xfs
	make -C $(XFS_IMGWRP_DIR)

distclean: clean-afl-llvm-mode clean-afl clean-syscall clean-ffgcc
