AC_PREREQ(2.59)
AC_INIT(seed, 0.3)

AM_INIT_AUTOMAKE([1.7 -Wno-portability])
AM_MAINTAINER_MODE
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

CFLAGS="-O3"

changequote(,)dnl
ensureflag() {
  flag="$1"; shift
  result="$@"

  case " ${result} " in
  *[\ \	]${flag}[\ \	]*) ;;
  *) result="${flag} ${result}" ;;
  esac

  echo ${result}
}

changequote([,])dnl


dnl =====================Internationalization==================================
GETTEXT_PACKAGE=seed
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])

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

dnl =======================WebKit ContextGroup=================================
case "$host_os" in
	darwin*)
		;;
	*)
		save_LIBS=$LIBS
		LIBS=`pkg-config --libs webkit-1.0`
		AC_CHECK_LIB(webkit-1.0, JSContextGroupCreate,,
					 AC_MSG_ERROR([The installed version of WebKit is too old. Seed requires WebKit SVN revision 35442 or greater.]))
		LIBS=$save_LIBS
		;;
esac

dnl ==============================readline=====================================
AC_MSG_CHECKING(for readline)
AC_TRY_CPP([#include <readline/readline.h>],
			have_readline_h=yes,
			have_readline_h=no)
AC_MSG_RESULT([$have_readline_h])

dnl ==============================cairo========================================
AC_MSG_CHECKING(for cairo)
AC_TRY_CPP([#include <cairo/cairo.h>], have_cairo_h=yes, have_cairo_h=no)
AC_MSG_RESULT([$have_cairo_h])

dnl ============================sqlite3========================================
AC_MSG_CHECKING(for sqlite3)
AC_TRY_CPP([#include <sqlite3.h>], have_sqlite3_h=yes, have_sqlite3_h=no)
AC_MSG_RESULT([$have_sqlite3_h])

dnl =============================gtk+-2.0======================================
AC_MSG_CHECKING(for gtk+-2.0)
PKG_CHECK_EXISTS(gtk+-2.0, have_gtk=yes, have_gtk=no)
AC_MSG_RESULT([$have_gtk])

dnl =============================Modules=======================================
AC_ARG_ENABLE(canvas-module,
			  AC_HELP_STRING([--enable-canvas-module],
							 [enable the canvas Seed module. [default=yes]]),
			  [want_canvas_module=$enableval],[want_canvas_module="yes"])

AC_ARG_ENABLE(example-module,
			  AC_HELP_STRING([--enable-example-module],
							 [enable the example Seed module. [default=yes]]),
			  [want_example_module=$enableval],[want_example_module="yes"])

AC_ARG_ENABLE(multiprocessing-module,
			  AC_HELP_STRING([--enable-multiprocessing-module],
							 [enable the multiprocessing Seed module. [default=yes]]),
			  [want_multiprocessing_module=$enableval],
			  [want_multiprocessing_module="yes"])

AC_ARG_ENABLE(readline-module,
			  AC_HELP_STRING([--enable-readline-module],
							 [enable the readline Seed module. [default=yes]]),
			  [want_readline_module=$enableval],[want_readline_module="yes"])

AC_ARG_ENABLE(sqlite-module,
			  AC_HELP_STRING([--enable-sqlite-module],
							 [enable the sqlite Seed module. [default=yes]]),
			  [want_sqlite_module=$enableval],[want_sqlite_module="yes"])

dnl --- Special Cases ---

if test x"$have_cairo_h" != x"yes" ; then
	want_canvas_module="no"
fi

if test x"$have_readline_h" != x"yes" ; then
	want_readline_module="no"
fi

if test x"$have_sqlite3_h" != x"yes" ; then
	want_sqlite_module="no"
fi

AM_CONDITIONAL(BUILD_CANVAS_MODULE, test "x$want_canvas_module" = "xyes")
AM_CONDITIONAL(BUILD_EXAMPLE_MODULE, test "x$want_example_module" = "xyes")
AM_CONDITIONAL(BUILD_MULTIPROCESSING_MODULE, test "x$want_multiprocessing_module" = "xyes")
AM_CONDITIONAL(BUILD_READLINE_MODULE, test "x$want_readline_module" = "xyes")
AM_CONDITIONAL(BUILD_SQLITE_MODULE, test "x$want_sqlite_module" = "xyes")

AC_SUBST(BUILD_CANVAS_MODULE)
AC_SUBST(BUILD_EXAMPLE_MODULE)
AC_SUBST(BUILD_MULTIPROCESSING_MODULE)
AC_SUBST(BUILD_READLINE_MODULE)
AC_SUBST(BUILD_SQLITE_MODULE)

dnl =========================turtle example====================================
AC_ARG_ENABLE(turtle-example,
			  AC_HELP_STRING([--enable-turtle-example],
							 [enable the turtle graphics example. [default=yes]]),
			  [want_turtle_example=$enableval],[want_turtle_example="yes"])

if test x"$have_gtk" != x"yes" ; then
	want_turtle_example="no"
fi

AM_CONDITIONAL(BUILD_TURTLE_EXAMPLE, test "x$want_turtle_example" = "xyes")
AC_SUBST(BUILD_TURTLE_EXAMPLE)

dnl =========================Install Examples==================================
AC_ARG_ENABLE(examples,
			  AC_HELP_STRING([--enable-examples],
							 [install JavaScript examples. [default=yes]]),
			  [enable_examples=$enableval],[enable_examples="yes"])
AM_CONDITIONAL(BUILD_SEED_EXAMPLES, test "x$enable_examples" = "xyes")
AC_SUBST(BUILD_SEED_EXAMPLES)

dnl =============================Debug=========================================
AC_ARG_ENABLE(debug,
			  AC_HELP_STRING([--enable-debug],
							 [enable verbose debugging. [default=no]]),
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

AC_ARG_ENABLE(profile-modules,
			  AC_HELP_STRING([--enable-profile-modules],
							 [turn on profiling for modules [default=no]]),
			  [enable_profile_modules="yes"],[enable_profile_modules="no"])

if test "x$enable_profile" = "xyes"; then
	SEED_PROFILE_CFLAGS="-fprofile-arcs -ftest-coverage"
	SEED_PROFILE_LDFLAGS="-lgcov"
	CFLAGS="-O0"
fi

if test "x$enable_profile" = "xno"; then
	[enable_profile_modules="no"]
fi

AC_SUBST(SEED_PROFILE_CFLAGS)
AC_SUBST(SEED_PROFILE_LDFLAGS)

AM_CONDITIONAL(PROFILE_MODULES, test "x$enable_profile_modules" = "xyes")

dnl ===========================Darwin Build====================================
SEED_OSX_CFLAGS=
SEED_OSX_LDFLAGS=

case "$host_os" in
	darwin*)
		SEED_OSX_CFLAGS="-F/Applications/WebKit.app/Contents/Frameworks/10.5 -framework JavaScriptCore"
		SEED_OSX_LDFLAGS="-F/Applications/WebKit.app/Contents/Frameworks/10.5 -framework JavaScriptCore"
		PKG_CHECK_MODULES(SEED, gobject-introspection-1.0)
		PKG_CHECK_MODULES(LIBSEED, gobject-introspection-1.0)
		;;
	*)
		PKG_CHECK_MODULES(SEED, webkit-1.0 gobject-introspection-1.0 libffi)
		PKG_CHECK_MODULES(LIBSEED, webkit-1.0 gobject-introspection-1.0 libffi)
		;;
esac
AC_SUBST(SEED_OSX_CFLAGS)
AC_SUBST(SEED_OSX_LDFLAGS)

dnl =============================gtk-doc=======================================
GTK_DOC_CHECK(1.9)

dnl =============================Output========================================
SHAVE_INIT([scripts])
AC_CONFIG_FILES([
seed.pc
Makefile

doc/conventions.html
doc/mapping.html
doc/runtime.html
doc/tutorial-standalone/tutorial.html
doc/modules/canvas.html
doc/modules/multiprocessing.html
doc/modules/readline.html
doc/modules/sqlite.html

scripts/shave
scripts/shave-libtool

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
examples/clutter-transitions/Makefile
examples/glib/Makefile
examples/browser/Makefile
examples/turtle/Makefile
examples/opengl/Makefile
examples/twitter/Makefile
modules/Makefile
modules/example/Makefile
modules/sqlite/Makefile
modules/canvas/Makefile
modules/readline/Makefile
modules/Multiprocessing/Makefile
])
AC_OUTPUT

echo "
Build Configuration:
   Debug......................$enable_debug
   Profiling/Coverage.........$enable_profile
   Profiling for Modules......$enable_profile_modules
   gtk-doc....................$enable_gtk_doc

Installation:
   Prefix.....................$prefix
   Install examples...........$enable_examples

Modules:
   Canvas.....................$want_canvas_module
   example....................$want_example_module
   multiprocessing............$want_multiprocessing_module
   readline...................$want_readline_module
   SQLite.....................$want_sqlite_module

Examples:
   Turtle.....................$want_turtle_example
"

if test "x$want_readline_module" != "xyes"; then
echo "WARNING: The readline module is not enabled.
         Without this, the Seed REPL will not work;
         however, most Seed applications will still run.
"
fi
