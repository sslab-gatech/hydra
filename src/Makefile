CUR_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

include $(CUR_DIR)/Makefile.inc

LLVM_ARGS := -DLLVM_TARGETS_TO_BUILD=X86 -DCMAKE_BUILD_TYPE=Release

all: readme consistency-readme

readme:
	@cat build-note

consistency-readme:
	@echo ""
	@echo "== Running the consistency checker"
	@cat consistency-run-note

build-llvm:
	if [ ! -d "$(LLVM_BLD)" ]; then \
		mkdir -p $(LLVM_BLD); \
		cd $(LLVM_BLD) && \
		cmake $(LLVM_ARGS) $(LLVM_DIR); \
		@cd ..; \
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

build-lkl-vfat: get-lkl
	(cd $(LKL_DIR) && ./compile -t fat -c && cd -)

build-lkl-sdcardfs: get-lkl
	(cd $(LKL_DIR) && ./compile -t sdcardfs -c && cd -)

get-lkl: build-img-sys
	if [ ! -d "$(LKL_DIR)" ]; then \
		git clone git@tc.gtisc.gatech.edu:lkl-new $(LKL_DIR); \
	fi

build-btrfs-imgwrp: build-lkl-btrfs
	make -C $(BTRFS_IMGWRP_DIR)

build-ext4-imgwrp: build-ext4-e2fsprogs
	make -C $(EXT4_IMGWRP_DIR)

build-ext4-e2fsprogs: build-lkl-ext4
	(cd $(EXT4_LIB_DIR); \
	 CFLAGS="-fPIC" CPPFLAGS="-fPIC" LDFLAGS="-lblkid -luuid" ./configure --enable-libblkid --enable-libuuid)
	make -C $(EXT4_LIB_DIR)

build-f2fs-imgwrp: build-lkl-f2fs
	make -C $(F2FS_IMGWRP_DIR)

build-xfs-imgwrp: build-lkl-xfs
	make -C $(XFS_IMGWRP_DIR)

build-vfat-imgwrp: build-lkl-vfat
	make -C $(FAT_IMGWRP_DIR)

build-sdcardfs-imgwrp: build-lkl-sdcardfs
	make -C $(SDCARDFS_IMGWRP_DIR)

distclean: clean-afl-llvm-mode clean-afl clean-syscall clean-ffgcc
