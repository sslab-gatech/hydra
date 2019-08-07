AC_DEFUN([AC_PACKAGE_NEED_NCURSES_H],
  [ AC_CHECK_HEADERS([ncurses.h])
    if test "$ac_cv_header_ncurses_h" != yes; then
        echo
        echo 'FATAL ERROR: could not find a valid ncurses header.'
        echo 'Install the ncurses development package.'
        exit 1
    fi
  ])

AC_DEFUN([AC_PACKAGE_WANT_WORKING_LIBNCURSES],
  [ AC_CHECK_LIB(ncurses, initscr,, [
        echo
        echo 'FATAL ERROR: could not find a valid ncurses library.'
        echo 'Install the ncurses library package.'
        exit 1
    ])
    AC_MSG_CHECKING([for bad glibc/ncurses header interaction])
    libcurses="-lncurses"
    LIBS="$LIBS $libcurses"
    CFLAGS="$CFLAGS -D_GNU_SOURCE"
    AC_TRY_LINK([
#include <ncurses.h>
#include <signal.h>],
        [wgetch(stdscr);],
        [enable_curses=yes; AC_MSG_RESULT([ok])],
        [enable_curses=no; libcurses=""; AC_MSG_RESULT([disabling curses])])
    AC_SUBST(enable_curses)
    AC_SUBST(libcurses)
  ])
