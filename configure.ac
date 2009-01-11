AC_INIT(seed, 0.3)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_CONFIG_MACRO_DIR(m4)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

CFLAGS="-O3"

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

dnl ========================readline module====================================
AC_TRY_CPP([#include <readline/readline.h>],
					 have_readline_h=yes,
					 have_readline_h=no)
AM_CONDITIONAL(BUILD_READLINE_MODULE, test "x$have_readline_h" = "xyes")
AC_SUBST(BUILD_READLINE_MODULE)

dnl =========================canvas module=====================================
AC_TRY_CPP([#include <cairo/cairo.h>], have_cairo_h=yes, have_cairo_h=no)
AM_CONDITIONAL(BUILD_CANVAS_MODULE, test "x$have_cairo_h" = "xyes")
AC_SUBST(BUILD_CANVAS_MODULE)

dnl =========================sqlite module=====================================
AC_TRY_CPP([#include <sqlite3.h>], have_sqlite3_h=yes, have_sqlite3_h=no)
AM_CONDITIONAL(BUILD_SQLITE3_MODULE, test "x$have_sqlite3_h" = "xyes")
AC_SUBST(BUILD_SQLITE3_MODULE)

dnl =========================Install Examples==================================
AC_ARG_ENABLE(examples,
              AC_HELP_STRING([--enable-examples],
                             [build and install Javascript examples. [default=yes]]),
                             [enable_examples=$enableval],[enable_examples="yes"])
AM_CONDITIONAL([BUILD_SEED_EXAMPLES], [test "x$enable_examples"="xyes"])
AC_SUBST(BUILD_SEED_EXAMPLES)

dnl =============================Debug=========================================
AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [build debugging code. This does not actually enable debugging, it only builds the code in. Pass --seed-debug=(misc/finalization/initialization/signal/all) to enable debugging. [default=no]]),
              [enable_debug="yes"],[enable_debug="no"])

if test "x$enable_debug" = "xyes"; then
  SEED_DEBUG_CFLAGS="-DSEED_ENABLE_DEBUG -g"
  CFLAGS="-O0"
else
  SEED_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
fi

AC_SUBST(SEED_DEBUG_CFLAGS)

dnl ===========================Profiling=======================================
AC_ARG_ENABLE(profile,
              AC_HELP_STRING([--enable-profile],
                             [turn on profiling [default=no]]),
              [enable_profile="yes"],[enable_profile="no"])

if test "x$enable_profile" = "xyes"; then
  SEED_PROFILE_CFLAGS="-fprofile-arcs -ftest-coverage"
  SEED_PROFILE_LDFLAGS="-lgcov"
  CFLAGS="-O0"
fi

AC_SUBST(SEED_PROFILE_CFLAGS)
AC_SUBST(SEED_PROFILE_LDFLAGS)

dnl ===========================Darwin Build====================================
SEED_OSX_CFLAGS=
SEED_OSX_LDFLAGS=

SEED_OSX_CFLAGS="-F/Applications/WebKit.app/Contents/Frameworks/10.5 -framework JavaScriptCore"
SEED_OSX_LDFLAGS="-F/Applications/WebKit.app/Contents/Frameworks/10.5 -framework JavaScriptCore"

AC_SUBST(SEED_OSX_CFLAGS)
AC_SUBST(SEED_OSX_LDFLAGS)

dnl ===========================Packages========================================
PKG_CHECK_MODULES(SEED, gobject-introspection-1.0)
PKG_CHECK_MODULES(LIBSEED, gobject-introspection-1.0)

AC_CONFIG_FILES([
seed.pc])

dnl =============================gtk-doc=======================================
GTK_DOC_CHECK(1.9)

dnl =============================Output========================================
AC_OUTPUT([
Makefile

doc/conventions.html
doc/runtime.html
doc/tutorial-standalone/tutorial.html
doc/modules/canvas.html
doc/modules/multiprocessing.html
doc/modules/readline.html
doc/modules/sqlite.html

libseed/Makefile
po/Makefile.in
src/Makefile
extensions/Makefile
tests/Makefile
tests/javascript/Makefile
tests/c/Makefile
doc/Makefile
doc/modules/Makefile
doc/tutorial-standalone/Makefile
doc/reference/Makefile
examples/Makefile
examples/clutter-shader/Makefile
examples/glib/Makefile
examples/browser/Makefile
examples/turtle/Makefile
modules/Makefile
modules/example/Makefile
modules/sqlite/Makefile
modules/canvas/Makefile
modules/readline/Makefile
modules/Multiprocessing/Makefile
])

echo "
Build Configuration:
   Debug......................$enable_debug
   Profiling/Coverage.........$enable_profile
   gtk-doc....................$enable_gtk_doc

Installation:
   Prefix.....................$prefix
   Install examples...........$enable_examples

Modules:
   SQLite.....................$have_sqlite3_h
   Canvas.....................$have_cairo_h
   readline...................$have_readline_h
"
