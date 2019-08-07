dnl Process this file with autoconf to produce a configure script.
AC_INIT([reiserfsprogs], [3.6.27], [reiserfs-devel@vger.kernel.org])
AC_CANONICAL_HOST
AC_PREREQ(2.50)

AH_TEMPLATE([HAVE_LIBUUID], [Define for enable libuuid using.])

AM_INIT_AUTOMAKE([1.11.1 dist-xz silent-rules])
AM_SILENT_RULES([yes])
AM_MAINTAINER_MODE

dnl We install in /sbin, the utils are to be available on boot
dnl AC_PREFIX(/)

AM_CONFIG_HEADER(include/config.h)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_AWK 
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl Checks for libraries.

# Check for LIBUUID
AC_CHECK_LIB(uuid, uuid_generate, , AC_MSG_WARN(libuuid could not be found))
UUID_LIBS="$LIBS"
AC_SUBST(UUID_LIBS)

AC_CHECK_LIB(com_err, _et_list, , AC_MSG_ERROR(libcom_err could not be found))
COM_ERR_LIBS="$LIBS"
AC_SUBST(COM_ERR_LIBS)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h limits.h malloc.h sys/ioctl.h unistd.h uuid/uuid.h)
AC_HEADER_MAJOR

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_ST_RDEV

# Checking for compiler warning options
AC_CACHE_CHECK([whether -Wuninitialized works], [ac_cv_wuninitialized_flag], [
	saved_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="-O1 -Wuninitialized"
	AC_TRY_COMPILE(, , [ac_cv_wuninitialized_flag=yes], [ac_cv_wuninitialized_flag=no])
	CPPFLAGS="$saved_CPPFLAGS"
])

AC_CACHE_CHECK([whether -Wno-unused-parameter works], [ac_cv_wno_unused_parameter_flag], [
	saved_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="-Wno-unused-parameter"
	AC_TRY_COMPILE(, , [ac_cv_wno_unused_parameter_flag=yes], [ac_cv_wno_unused_parameter_flag=no])
	CPPFLAGS="$saved_CPPFLAGS"
])

AC_CACHE_CHECK([whether -Wredundant-decls works], [ac_cv_wredundant_decls_flag], [
	saved_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="-Wredundant-decls"
	AC_TRY_COMPILE(, , [ac_cv_wredundant_decls_flag=yes], [ac_cv_wredundant_decls_flag=no])
	CPPFLAGS="$saved_CPPFLAGS"
])

# Check for large file
AC_SYS_LARGEFILE

if test -z "${ac_cv_sys_file_offset_bits}"; then
	AC_MSG_WARN(Can't detect right _FILE_OFFSET_BITS. Will be forced to 64bit.)
	ac_cv_sys_file_offset_bits=64
fi

AC_CHECK_SIZEOF(off_t, 64, [ 
	#include <stdio.h>
	#include <sys/types.h>
	#include <unistd.h> 
])

AC_CHECK_SIZEOF(blkcnt_t, 64, [ 
	#include <stdio.h>
	#include <sys/types.h>
	#include <unistd.h> 
])

dnl Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(strerror strstr strtol statfs getmntent hasmntopt memset time \
	       uname strptime ctime_r register_printf_modifier \
	       register_printf_specifier)

if test -z "${ac_cv_func_register_printf_function}" -a -z "${ac_cv_func_register_printf_specifier}"; then
	AC_MSG_ERROR(reiserfsprogs requires a method to add printf modifiers)
fi


dnl Never enable this. It is for debugging only
AC_ARG_ENABLE(io-failure-emulation,
	[AS_HELP_STRING([--enable-io-failure-emulation], [Never use this. It is for debugging only])])
if test "$enable_io_failure_emulation" = "yes" ; then
	AC_MSG_WARN([Careful! IO failure emulation is ON])
	AC_DEFINE(IO_FAILURE_EMULATION, 1, [gets set when configure --enable-io-failure-emulation])
fi

if test "x$ac_cv_wno_unused_parameter_flag" = xyes; then
	CFLAGS="$CFLAGS -Wno-unused-parameter"
else
	CFLAGS="$CFLAGS -Wno-unused"
fi
	
if test "x$ac_cv_wredundant_decls_flag" = xyes; then
	CFLAGS="$CFLAGS -Wredundant-decls"
fi
	
if test "x$ac_cv_wuninitialized_flag" = xyes; then
	CFLAGS="$CFLAGS -Wuninitialized"
fi

CPPFLAGS="-I\$(top_srcdir)/include $CPPFLAGS"

AC_OUTPUT(
    include/Makefile 
    mkreiserfs/Makefile 
    mkreiserfs/mkreiserfs.8
    resize_reiserfs/Makefile 
    resize_reiserfs/resize_reiserfs.8
    fsck/Makefile 
    fsck/reiserfsck.8
    lib/Makefile 
    Makefile 
    reiserfscore/Makefile 
    reiserfscore/reiserfscore.pc
    debugreiserfs/Makefile 
    debugreiserfs/debugreiserfs.8
    tune/Makefile
    tune/reiserfstune.8
    )

AC_MSG_NOTICE([

Type 'make' to compile reiserfsprogs])
