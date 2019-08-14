PROJ_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

LKL_DIR         := $(PROJ_DIR)/lkl
LLVM_DIR        := $(PROJ_DIR)/llvm
AFL_DIR         := $(PROJ_DIR)/afl
AFL_LLVM_DIR    := $(AFL_DIR)/llvm_mode
SYSCALL_DIR     := $(PROJ_DIR)/syscall
FF_GCC_DIR      := $(PROJ_DIR)/ff-gcc
AFL_CMB_DIR     := $(PROJ_DIR)/combined
AFL_IMG_SYS_DIR := $(AFL_CMB_DIR)/afl-image-syscall

BTRFS_IMGWRP_DIR    := $(PROJ_DIR)/fs/btrfs
EXT4_IMGWRP_DIR     := $(PROJ_DIR)/fs/ext4
EXT4_LIB_DIR        := $(EXT4_IMGWRP_DIR)/e2fsprogs
F2FS_IMGWRP_DIR     := $(PROJ_DIR)/fs/f2fs
GFS2_IMGWRP_DIR     := $(PROJ_DIR)/fs/gfs2
HFSPLUS_IMGWRP_DIR  := $(PROJ_DIR)/fs/hfsplus
RSR_IMGWRP_DIR      := $(PROJ_DIR)/fs/reiserfs
VFAT_IMGWRP_DIR     := $(PROJ_DIR)/fs/vfat
SDCARDFS_IMGWRP_DIR := $(PROJ_DIR)/fs/sdcardfs
XFS_IMGWRP_DIR      := $(PROJ_DIR)/fs/xfs

LLVM_BLD  := $(PROJ_DIR)/llvm-build
CLANG     := $(LLVM_BLD)/bin/clang
CLANG_CPP := $(LLVM_BLD)/bin/clang++
LLVM_CONF := $(LLVM_BLD)/bin/llvm-config

ifeq ($(strip $(NJOB)), )
  NJOB := ${shell nproc}
endif
