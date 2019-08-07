AC_INIT([xfsdump], [3.1.8])
AC_PREREQ(2.50)
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([common/main.c])
AC_CONFIG_HEADER(include/config.h)
AC_PREFIX_DEFAULT(/usr)

AC_SYS_LARGEFILE

AC_PROG_LIBTOOL

AC_ARG_ENABLE(shared,
[ --enable-shared=[yes/no] Enable use of shared libraries [default=yes]],,
	enable_shared=yes)
AC_SUBST(enable_shared)

AC_ARG_ENABLE(gettext,
[ --enable-gettext=[yes/no] Enable alternate language support [default=yes]],,
	enable_gettext=yes)
AC_SUBST(enable_gettext)

AC_ARG_ENABLE(lib64,
[ --enable-lib64=[yes/no] Enable lib64 support [default=yes]],,
	enable_lib64=yes)
AC_SUBST(enable_lib64)

#
# If the user specified a libdir ending in lib64 do not append another
# 64 to the library names.
#
base_libdir=`basename "$libdir"`
case $base_libdir in
lib64)
  enable_lib64=no
esac

#
# Some important tools should be installed into the root partitions.
#
# Check whether exec_prefix=/usr: and install them to /sbin in that
# case.  If the user choses a different prefix assume he just wants
# a local install for testing and not a system install.
#
case $exec_prefix:$prefix in
NONE:NONE | NONE:/usr | /usr:*)
  root_sbindir='/sbin'
  root_libdir="/${base_libdir}"
  ;;
*)
  root_sbindir="${sbindir}"
  root_libdir="${libdir}"
  ;;
esac

AC_SUBST([root_sbindir])
AC_SUBST([root_libdir])

# Find localized files
LOCALIZED_FILES=""
for lfile in `find ${srcdir} -name '*.c' -type f || exit 1`; do
    LOCALIZED_FILES="$LOCALIZED_FILES \$(TOPDIR)/$lfile"
done
AC_SUBST(LOCALIZED_FILES)

AC_PACKAGE_GLOBALS(xfsdump)
AC_PACKAGE_UTILITIES(xfsdump)
AC_MULTILIB($enable_lib64)

AC_PACKAGE_NEED_UUID_H
AC_PACKAGE_NEED_UUIDCOMPARE

AC_PACKAGE_NEED_PTHREAD_H
AC_PACKAGE_NEED_PTHREADMUTEXINIT

AC_PACKAGE_NEED_NCURSES_H
AC_PACKAGE_WANT_WORKING_LIBNCURSES

AC_PACKAGE_NEED_XFS_XFS_H
AC_PACKAGE_NEED_XFS_HANDLE_H
AC_PACKAGE_NEED_OPEN_BY_FSHANDLE

AC_PACKAGE_NEED_ATTRIBUTES_H
AC_PACKAGE_NEED_ATTRIBUTES_MACROS
AC_PACKAGE_NEED_ATTRGET_LIBATTR

AC_MANUAL_FORMAT

AC_CONFIG_FILES([include/builddefs])
AC_OUTPUT
