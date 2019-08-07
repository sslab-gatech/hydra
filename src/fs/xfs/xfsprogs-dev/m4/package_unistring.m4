AC_DEFUN([AC_PACKAGE_WANT_UNINORM_H],
  [ AC_CHECK_HEADERS(uninorm.h)
    if test $ac_cv_header_uninorm_h = no; then
	AC_CHECK_HEADERS(uninorm.h,, [
	echo
	echo 'WARNING: could not find a valid uninorm.h header.'])
    fi
  ])

AC_DEFUN([AC_HAVE_U8NORMALIZE],
  [ AC_CHECK_LIB(unistring, u8_normalize,[
	libunistring=-lunistring
	have_u8normalize=yes
    ],[
	echo
	echo 'WARNING: xfs_scrub will not be built with Unicode libraries.'])
    AC_SUBST(libunistring)
    AC_SUBST(have_u8normalize)
  ])
