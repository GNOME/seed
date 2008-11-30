AC_INIT(seed, 0.1.2)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

dnl =====================Internationalization==================================
GETTEXT_PACKAGE=seed
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])

AM_PROG_LIBTOOL

dnl ============================libffi=========================================
PKG_CHECK_MODULES(libffi, libffi, have_ffi_pkgconfig=yes, have_ffi_pkgconfig=no)

if test x"$have_ffi_pkgconfig" = xno ; then
  AC_MSG_CHECKING(for ffi.h)

  AC_TRY_CPP([#include <ffi.h>], have_ffi_h=yes, have_ffi_h=no)
  if test x"$have_ffi_h" = x"yes"; then

    save_LIBS=$LIBS
    if test x"$with_ffi" = x"yes" || test x"$with_ffi" = x"auto"; then
      other_LIBS=
    else
      other_LIBS=$with_ffi
    fi

    AC_SEARCH_LIBS(ffi_call,ffi,,AC_MSG_ERROR([libffi not found]),$other_LIBS)
    if test x"$ac_cv_search_ffi_call" = x"none required" ; then
      FFI_LIBS=$other_LIBS
    else
      FFI_LIBS="$ac_cv_search_ffi_call $other_LIBS"
    fi

    LIBS=$save_LIBS
  fi
  if test x"$have_ffi_h" != x"yes" ; then
    AC_MSG_ERROR([ffi.h not found])
  fi

  FFI_CFLAGS=

  AC_MSG_RESULT([$have_ffi_h])
  AC_SUBST(FFI_LIBS)
  AC_SUBST(FFI_CFLAGS)
fi

dnl ==========================libreadline======================================
AC_CHECK_LIB(readline, readline,
    AC_DEFINE(HAVE_LIBREADLINE, 1, [have readline]),
    AC_MSG_ERROR([readline not found]))

dnl =============================Debug=========================================
AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [build debugging code. This does not actually enable debugging, it only builds the code in. Pass --seed-debug=(misc/finalization/initialization/signal/all) to enable debugging. [default=no]]),
              [enable_debug="yes"],[enable_debug="no"])

if test "x$enable_debug" = "xyes"; then
  SEED_DEBUG_CFLAGS="-DSEED_ENABLE_DEBUG -g -O0"
else
  SEED_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS -g -O0"
fi

AC_SUBST(SEED_DEBUG_CFLAGS)

dnl ===========================Profiling=======================================
AC_ARG_ENABLE(profile,
              AC_HELP_STRING([--enable-profile],
                             [turn on profiling [default=no]]),
              [enable_profile="yes"],[enable_profile="no"])

if test "x$enable_profile" = "xyes"; then
  SEED_PROFILE_CFLAGS="-fprofile-arcs -ftest-coverage -O0"
  SEED_PROFILE_LDFLAGS="-lgcov"
fi

AC_SUBST(SEED_PROFILE_CFLAGS)
AC_SUBST(SEED_PROFILE_LDFLAGS)

dnl ===========================Packages========================================
PKG_CHECK_MODULES(SEED, gobject-introspection-1.0 webkit-1.0)
PKG_CHECK_MODULES(LIBSEED, gobject-introspection-1.0 webkit-1.0)

AC_CONFIG_FILES([
seed.pc])

dnl =============================Output========================================
AC_OUTPUT([
Makefile
libseed/Makefile
po/Makefile.in
src/Makefile
extensions/Makefile
tests/Makefile
doc/Makefile
doc/tutorial-standalone/Makefile
examples/Makefile
examples/ide/Makefile
examples/lightsoff/Makefile
examples/clutter-shader/Makefile
modules/Makefile
modules/example/Makefile
modules/sqlite/Makefile
])

echo "
Build Configuration:
   Debug......................$enable_debug
   Profiling/Coverage:........$enable_profile
"
