
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([gfs2-utils], [master], [cluster-devel@redhat.com])
AM_INIT_AUTOMAKE([-Wno-portability])
AM_SILENT_RULES([yes])
LT_PREREQ([2.2.6])
LT_INIT

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([gfs2/libgfs2/libgfs2.h])
AC_CONFIG_HEADERS([make/clusterautoconfig.h])

AC_CANONICAL_HOST
AC_PROG_LIBTOOL

AC_LANG([C])

#i18n support
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18])

# Sanitize path

if test "$prefix" = "NONE"; then
	prefix="/usr"
	if test "$localstatedir" = "\${prefix}/var"; then
		localstatedir="/var"
	fi
	if test "$sysconfdir" = "\${prefix}/etc"; then
		sysconfdir="/etc"
	fi
	if test "$libdir" = "\${exec_prefix}/lib"; then
		if test -e /usr/lib64; then
			libdir="/usr/lib64"
		else
			libdir="/usr/lib"
		fi
	fi
fi

case $exec_prefix in
  NONE)   exec_prefix=$prefix;;
  prefix) exec_prefix=$prefix;;
esac

# Checks for programs.

# check stolen from gnulib/m4/gnu-make.m4
if ! ${MAKE-make} --version /cannot/make/this >/dev/null 2>&1; then
	AC_MSG_ERROR([GNU make was not found but is required.])
fi

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LEX
test "$LEX" != "flex" && AC_MSG_ERROR([flex not found])
AC_CHECK_PROG([YACC], [bison], [bison -y])
test x"$YACC" = x && AC_MSG_ERROR([bison not found])

## local helper functions

# this function checks if CC support options passed as
# args. Global CFLAGS are ignored during this test.
cc_supports_flag() {
	local CFLAGS="$@"
	AC_MSG_CHECKING([whether $CC supports $CFLAGS])
	AC_COMPILE_IFELSE([AC_LANG_SOURCE([int main(){return 0;}])],
			  [RC=0; AC_MSG_RESULT([yes])],
			  [RC=1; AC_MSG_RESULT([no])])
	return $RC
}

# this function tests if a library has a certain function
# by using AC_CHECK_LIB but restores the original LIBS global
# envvar. This is required to avoid libtool to link everything
# with everything.
check_lib_no_libs() {
	AC_CHECK_LIB([$1], [$2],,
		     [AC_MSG_ERROR([Unable to find $1 library])])
	LIBS=$ac_check_lib_save_LIBS
}

# local options
AC_ARG_ENABLE([debug],
	AC_HELP_STRING([--enable-debug],[enable debug build [default=no]]),
	[], [enable_debug="no"])
AC_ARG_ENABLE([gcov],
	AC_HELP_STRING([--enable-gcov],[enable coverage instrumentation [default=no]]),
	[], [enable_gcov="no"])
AC_ARG_ENABLE([gprof],
	AC_HELP_STRING([--enable-gprof],[enable profiling instrumentation [default=no]]),
	[], [enable_gprof="no"])


# We use the Check framework for unit tests
PKG_CHECK_MODULES([check], [check >= 0.9.8],
                  [have_check=yes],
                  [have_check=no])
AM_CONDITIONAL([HAVE_CHECK], [test "x$have_check" = "xyes"])

PKG_CHECK_MODULES([zlib],[zlib])
PKG_CHECK_MODULES([blkid],[blkid])
PKG_CHECK_MODULES([uuid],[uuid],
                  [have_uuid=yes],
                  [have_uuid=no])

# old versions of ncurses don't ship pkg-config files
PKG_CHECK_MODULES([ncurses],[ncurses],,
		  [check_lib_no_libs ncurses printw])

if test -z "$ncurses_CFLAGS"  && test -z "$ncurses_LIBS"; then
	ncurses_LIBS=-lncurses
fi

AC_ARG_WITH([udevdir],
            AS_HELP_STRING([--with-udevdir=DIR],
                           [udev directory containing rules.d [default=${prefix}/lib/udev]]),
            [], [with_udevdir=\${prefix}/lib/udev])
AC_SUBST([udevdir], [$with_udevdir])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h libintl.h limits.h locale.h mntent.h stddef.h sys/file.h sys/ioctl.h sys/mount.h sys/time.h sys/vfs.h syslog.h termios.h])
AC_CHECK_HEADER([linux/fs.h], [], [AC_MSG_ERROR([Unable to find linux/fs.h])])
AC_CHECK_HEADER([linux/types.h], [], [AC_MSG_ERROR([Unable to find linux/types.h])])
AC_CHECK_HEADER([linux/limits.h], [], [AC_MSG_ERROR([Unable to find linux/limits.h])])
AC_CHECK_HEADER([linux/gfs2_ondisk.h], [], [AC_MSG_ERROR([Unable to find linux/gfs2_ondisk.h])])
AC_CHECK_MEMBER([struct gfs2_sb.sb_uuid], [sb_has_uuid=yes], [sb_has_uuid=no],
                [[#include <linux/gfs2_ondisk.h>]])
AC_CHECK_MEMBER([struct gfs2_leaf.lf_inode],[AC_DEFINE([GFS2_HAS_LEAF_HINTS],[],[Leaf block hints])],
                [], [[#include <linux/gfs2_ondisk.h>]])
AC_CHECK_MEMBER([struct gfs2_dirent.de_rahead],[AC_DEFINE([GFS2_HAS_DE_RAHEAD],[],[Dirent readahead field])],
                [], [[#include <linux/gfs2_ondisk.h>]])
AC_CHECK_MEMBER([struct gfs2_dirent.de_cookie],[AC_DEFINE([GFS2_HAS_DE_COOKIE],[],[Dirent cookie field])],
                [], [[#include <linux/gfs2_ondisk.h>]])
AC_CHECK_MEMBER([struct gfs2_rgrp.rg_skip],[AC_DEFINE([GFS2_HAS_RG_SKIP],[],[Next resource group pointer])],
                [], [[#include <linux/gfs2_ondisk.h>]])
AC_CHECK_MEMBER([struct gfs2_rgrp.rg_data0],[AC_DEFINE([GFS2_HAS_RG_RI_FIELDS],[],[Resource group fields duplicated from the rindex])],
                [], [[#include <linux/gfs2_ondisk.h>]])
AC_CHECK_MEMBER([struct gfs2_log_header.lh_crc],[AC_DEFINE([GFS2_HAS_LH_V2],[],[v2 log header format])],
                [], [[#include <linux/gfs2_ondisk.h>]])

# libuuid is only required if struct gfs2_sb.sb_uuid exists
if test "$sb_has_uuid" = "yes" -a "$have_uuid" = "no"; then
	AC_MSG_ERROR([libuuid is required for this version of gfs2])
fi

# *FLAGS handling
ENV_CFLAGS="$CFLAGS"
ENV_CPPFLAGS="$CPPFLAGS"
ENV_LDFLAGS="$LDFLAGS"

# debug build stuff
if test "x${enable_debug}" = xyes; then
	AC_DEFINE_UNQUOTED([DEBUG], [1], [Compiling Debugging code])
	OPT_CFLAGS="-O0"
	OPT_CPPFLAGS=""
else
	OPT_CFLAGS="-O2"
	OPT_CPPFLAGS="-D_FORTIFY_SOURCE=2"
fi

# gdb flags
if test "x${GCC}" = xyes; then
	GDB_FLAGS="-ggdb3"
else
	GDB_FLAGS="-g"
fi

# gcov works without optimization
if test "x${enable_gcov}" = xyes; then
	GCOV_CFLAGS="-fprofile-arcs -ftest-coverage"
	if ! cc_supports_flag $GCOV_CFLAGS; then
		AC_MSG_ERROR([your compiler does not support coverage instrumentation])
	fi
	if test "x${enable_debug}" = xyes; then
		enable_debug="no (gcov enabled)"
	fi
	OPT_CFLAGS="-O0 $GCOV_CFLAGS"
fi

if test "x${enable_gprof}" = xyes; then
	GPROF_CFLAGS="-pg"
	if ! cc_supports_flag $GPROF_CFLAGS; then
		AC_MSG_ERROR([your compiler does not support gprof instrumentation])
	fi
	OPT_CFLAGS="$OPT_CFLAGS $GPROF_CFLAGS"
fi

# extra warnings
EXTRA_WARNINGS=""

WARNLIST="
	all
	shadow
	missing-prototypes
	missing-declarations
	strict-prototypes
	declaration-after-statement
	pointer-arith
	write-strings
	cast-align
	bad-function-cast
	missing-format-attribute
	format=2
	format-security
	format-nonliteral
	no-long-long
	no-strict-aliasing
	"

for j in $WARNLIST; do
	if cc_supports_flag -W$j; then
		EXTRA_WARNINGS="$EXTRA_WARNINGS -W$j";
	fi
done

CFLAGS="$ENV_CFLAGS $OPT_CFLAGS $GDB_FLAGS $EXTRA_WARNINGS $WERROR_CFLAGS"
CPPFLAGS="-I\$(top_builddir)/make -I\$(top_srcdir)/make -I. $ENV_CPPFLAGS $OPT_CPPFLAGS"
LDFLAGS="$ENV_LDFLAGS"

AC_CONFIG_TESTDIR([tests], [gfs2/libgfs2:gfs2/mkfs:gfs2/fsck:gfs2/edit:gfs2/convert:gfs2/tune:tests])
AC_CONFIG_FILES([Makefile
		 gfs2/Makefile
		 gfs2/include/Makefile
		 gfs2/libgfs2/Makefile
		 gfs2/convert/Makefile
		 gfs2/edit/Makefile
		 gfs2/fsck/Makefile
		 gfs2/mkfs/Makefile
		 gfs2/tune/Makefile
		 gfs2/man/Makefile
		 gfs2/scripts/Makefile
		 gfs2/glocktop/Makefile
		 doc/Makefile
		 tests/Makefile
		 tests/atlocal
		 po/Makefile.in
		 ])

AC_OUTPUT

echo
echo "  Configure summary"
echo " ==================="
echo " prefix            : $prefix"
echo " exec_prefix       : $exec_prefix"
echo " libdir            : $libdir"
echo " sbindir           : $sbindir"
echo " udevdir           : $udevdir"
echo " ------------------"
echo " debug build       : $enable_debug"
echo " C unit tests      : $have_check"
echo " gprof build       : $enable_gprof"
echo " gcov build        : $enable_gcov"
echo
echo "Now run 'make' to build and 'make check' to run tests"
