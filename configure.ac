AC_PREREQ(2.59)
AC_INIT(seed, 0.8.5)

AM_INIT_AUTOMAKE([1.7 -Wno-portability])
AM_MAINTAINER_MODE
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

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
PKG_CHECK_MODULES(FFI, libffi, have_ffi_pkgconfig=yes, have_ffi_pkgconfig=no)

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
			FFI_LDFLAGS=$other_LIBS
		else
			FFI_LDFLAGS="$ac_cv_search_ffi_call $other_LIBS"
		fi
		LIBS=$save_LIBS
	fi
	if test x"$have_ffi_h" != x"yes" ; then
		AC_MSG_ERROR([ffi.h not found])
	fi

	FFI_CFLAGS=

	AC_MSG_RESULT([$have_ffi_h])
fi

AC_SUBST(FFI_LDFLAGS)
AC_SUBST(FFI_CFLAGS)

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

dnl =========================Introspection=====================================
PKG_CHECK_MODULES(GOBJECT_INTROSPECTION, gobject-introspection-1.0 >= 0.6.3)
AC_SUBST(GOBJECT_INTROSPECTION_CFLAGS)
AC_SUBST(GOBJECT_INTROSPECTION_LDFLAGS)

dnl ========================GNOME-js common====================================
PKG_CHECK_MODULES(GNOME_JS, gnome-js-common)
GNOME_JS_DIR=`pkg-config --variable=moduledir gnome-js-common`
AC_SUBST(GNOME_JS_DIR)

dnl =============================Modules=======================================
dnl ==== canvas ====
AC_ARG_ENABLE(canvas-module,
			  AC_HELP_STRING([--enable-canvas-module],
							 [enable the canvas Seed module. [default=yes]]),
			  [want_canvas_module=$enableval],[want_canvas_module="yes"])

if test x"$want_canvas_module" == x"yes" ; then
	PKG_CHECK_MODULES(CAIRO, cairo)
	AC_SUBST(CAIRO_CFLAGS)
	AC_SUBST(CAIRO_LDFLAGS)
	PKG_CHECK_MODULES(GDK, gdk-2.0)
	AC_SUBST(GDK_CFLAGS)
	AC_SUBST(GDK_LDFLAGS)
fi

AM_CONDITIONAL(BUILD_CANVAS_MODULE, test "x$want_canvas_module" = "xyes")
AC_SUBST(BUILD_CANVAS_MODULE)



dnl ==== readline ====
AC_ARG_ENABLE(readline-module,
			  AC_HELP_STRING([--enable-readline-module],
							 [enable the readline Seed module. [default=yes]]),
			  [want_readline_module=$enableval],[want_readline_module="yes"])
if test x"$want_readline_module" == x"yes" ; then
	AC_CHECK_HEADER([readline/readline.h],,AC_MSG_ERROR([the readline library is not installed.]))
fi

AM_CONDITIONAL(BUILD_READLINE_MODULE, test "x$want_readline_module" = "xyes")
AC_SUBST(BUILD_READLINE_MODULE)

dnl ==== multiprocessing ====
AC_ARG_ENABLE(multiprocessing-module,
			  AC_HELP_STRING([--enable-multiprocessing-module],
							 [enable the multiprocessing Seed module. [default=yes]]),
			  [want_multiprocessing_module=$enableval],
			  [want_multiprocessing_module="yes"])

AM_CONDITIONAL(BUILD_MULTIPROCESSING_MODULE, test "x$want_multiprocessing_module" = "xyes")
AC_SUBST(BUILD_MULTIPROCESSING_MODULE)

dnl ==== sqlite ====
AC_ARG_ENABLE(sqlite-module,
			  AC_HELP_STRING([--enable-sqlite-module],
							 [enable the sqlite Seed module. [default=yes]]),
			  [want_sqlite_module=$enableval],[want_sqlite_module="yes"])

if test x"$want_sqlite_module" == x"yes" ; then
	PKG_CHECK_MODULES(SQLITE, sqlite3)
	AC_SUBST(SQLITE_CFLAGS)
	AC_SUBST(SQLITE_LDFLAGS)
fi

AM_CONDITIONAL(BUILD_SQLITE_MODULE, test "x$want_sqlite_module" = "xyes")
AC_SUBST(BUILD_SQLITE_MODULE)

dnl ==== Example ====
AC_ARG_ENABLE(example-module,
			  AC_HELP_STRING([--enable-example-module],
							 [enable the example Seed module. [default=yes]]),
			  [want_example_module=$enableval],[want_example_module="yes"])

AM_CONDITIONAL(BUILD_EXAMPLE_MODULE, test "x$want_example_module" = "xyes")
AC_SUBST(BUILD_EXAMPLE_MODULE)

dnl ==== dbus ====
AC_ARG_ENABLE(dbus-module,
			  AC_HELP_STRING([--enable-dbus-module],
							 [enable the dbus Seed module. [default=yes]]),
			  [want_dbus_module=$enableval],[want_dbus_module="yes"])


AM_CONDITIONAL(BUILD_DBUS_MODULE, test "x$want_dbus_module" = "xyes")
AC_SUBST(BUILD_DBUS_MODULE)

if test x"$want_dbus_module" == x"yes" ; then
	PKG_CHECK_MODULES(DBUS, dbus-1)
	AC_SUBST(DBUS_CFLAGS)
	AC_SUBST(DBUS_LDFLAGS)
	PKG_CHECK_MODULES(DBUSGLIB, dbus-glib-1)
	AC_SUBST(DBUSGLIB_CFLAGS)
	AC_SUBST(DBUSGLIB_LDFLAGS)
fi



dnl ==== os ====
AC_ARG_ENABLE(os-module,
			  AC_HELP_STRING([--enable-os-module],
							 [enable the os Seed module. [default=yes]]),
			  [want_os_module=$enableval],[want_os_module="yes"])


AM_CONDITIONAL(BUILD_OS_MODULE, test "x$want_os_module" = "xyes")
AC_SUBST(BUILD_OS_MODULE)

dnl ==== ffi ====
AC_ARG_ENABLE(ffi-module,
			  AC_HELP_STRING([--enable-ffi-module],
							 [enable the ffi Seed module. [default=yes]]),
			  [want_ffi_module=$enableval],[want_ffi_module="yes"])


AM_CONDITIONAL(BUILD_FFI_MODULE, test "x$want_ffi_module" = "xyes")
AC_SUBST(BUILD_FFI_MODULE)

dnl ==== libxml ====
AC_ARG_ENABLE(libxml-module,
			  AC_HELP_STRING([--enable-libxml-module],
							 [enable the libxml Seed module. [default=yes]]),
			  [want_libxml_module=$enableval],[want_libxml_module="yes"])

if test x"$want_libxml_module" == x"yes" ; then
   PKG_CHECK_MODULES(LIBXML, libxml-2.0)
   AC_SUBST(LIBXML_CFLAGS)
   AC_SUBST(LIBXML_LDFLAGS)
fi

AM_CONDITIONAL(BUILD_LIBXML_MODULE, test "x$want_libxml_module" = "xyes")
AC_SUBST(BUILD_LIBXML_MODULE)

dnl ==== os ====
AC_ARG_ENABLE(os-module,
			  AC_HELP_STRING([--enable-os-module],
							 [enable the os Seed module. [default=yes]]),
			  [want_os_module=$enableval],[want_os_module="yes"])


AM_CONDITIONAL(BUILD_OS_MODULE, test "x$want_os_module" = "xyes")
AC_SUBST(BUILD_OS_MODULE)

dnl ==== gtkbuilder ====
AC_ARG_ENABLE(gtkbuilder-module,
			  AC_HELP_STRING([--enable-gtkbuilder-module],
							 [enable the gtkbuilder Seed module. [default=yes]]),
			  [want_gtkbuilder_module=$enableval],[want_gtkbuilder_module="yes"])

if test x"$want_gtkbuilder_module" == x"yes" ; then
   PKG_CHECK_MODULES(GTK, gtk+-2.0)
   AC_SUBST(GTK_CFLAGS)
   AC_SUBST(GTK_LDFLAGS)
fi

AM_CONDITIONAL(BUILD_GTKBUILDER_MODULE, test "x$want_gtkbuilder_module" = "xyes")
AC_SUBST(BUILD_GTKBUILDER_MODULE)


dnl ==== cairo ====
AC_ARG_ENABLE(cairo-module,
			  AC_HELP_STRING([--enable-cairo-module],
							 [enable the cairo Seed module. [default=yes]]),
			  [want_cairo_module=$enableval],[want_cairo_module="yes"])

if test x"$want_cairo_module" == x"yes" ; then
	PKG_CHECK_MODULES(CAIRO, cairo)
	AC_SUBST(CAIRO_CFLAGS)
	AC_SUBST(CAIRO_LDFLAGS)
fi

AM_CONDITIONAL(BUILD_CAIRO_MODULE, test "x$want_cairo_module" = "xyes")
AC_SUBST(BUILD_CAIRO_MODULE)


dnl ==== gettext ====
AC_ARG_ENABLE(gettext-module,
			  AC_HELP_STRING([--enable-gettext-module],
							 [enable the gettext Seed module. [default=yes]]),
			  [want_gettext_module=$enableval],[want_gettext_module="yes"])

AM_CONDITIONAL(BUILD_GETTEXT_MODULE, test "x$want_gettext_module" = "xyes")
AC_SUBST(BUILD_GETTEXT_MODULE)

dnl ==== mpfr ====
AC_ARG_ENABLE(mpfr-module,
			  AC_HELP_STRING([--enable-mpfr-module],
							 [enable the mpfr Seed module. [default=yes]]),
			  [want_mpfr_module=$enableval],[want_mpfr_module="yes"])

if test x"$want_mpfr_module" == x"yes" ; then
	AC_TRY_CPP([#include <mpfr.h>], have_mpfr_h=yes, have_mpfr_h=no)
	if test x"$have_mpfr_h" = x"yes"; then
		save_LIBS=$LIBS
		if test x"$with_mpfr" = x"yes" || test x"$with_mpfr" = x"auto"; then
			other_LIBS=
		else
			other_LIBS=$with_mpfr
		fi

		AC_SEARCH_LIBS(mpfr_init2,mpfr,,AC_MSG_ERROR([mpfr not found]),$other_LIBS)
		if test x"$ac_cv_search_mpfr_call" = x"none required" ; then
			MPFR_LDFLAGS=$other_LIBS
		else
			MPFR_LDFLAGS="$other_LIBS"
		fi
		LIBS=$save_LIBS
	fi
	if test x"$have_mpfr_h" != x"yes" ; then
		AC_MSG_ERROR([mpfr.h not found])
	fi

	MPFR_CFLAGS=
fi

AM_CONDITIONAL(BUILD_MPFR_MODULE, test "x$want_mpfr_module" = "xyes")
AC_SUBST(BUILD_MPFR_MODULE)

dnl =========================turtle example====================================
AC_ARG_ENABLE(turtle-example,
			  AC_HELP_STRING([--enable-turtle-example],
							 [enable the turtle graphics example. [default=yes]]),
			  [want_turtle_example=$enableval],[want_turtle_example="yes"])

if test x"$want_turtle_example" == x"yes" ; then
	PKG_CHECK_MODULES(GTK, gtk+-2.0)
	AC_SUBST(GTK_CFLAGS)
	AC_SUBST(GTK_LIBS)
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
	SEED_PROFILE_LIBS="-lgcov"
	[need_debug="yes"]
fi

if test "x$enable_profile" = "xno"; then
	[enable_profile_modules="no"]
fi

AC_SUBST(SEED_PROFILE_CFLAGS)
AC_SUBST(SEED_PROFILE_LIBS)

AM_CONDITIONAL(PROFILE_MODULES, test "x$enable_profile_modules" = "xyes")

dnl =============================Debug=========================================
AC_ARG_ENABLE(debug,
			  AC_HELP_STRING([--enable-debug],
							 [enable verbose debugging. [default=no]]),
			  [enable_debug="yes"],[enable_debug="no"])

if test "x$enable_debug" = "xyes"; then
	SEED_DEBUG_CFLAGS="-DSEED_ENABLE_DEBUG -g"
	CFLAGS="-O0 -Wall"
else
	SEED_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
	CFLAGS="-Wall"
	
	if test "x$need_debug" = "xyes"; then
		AC_MSG_ERROR([debug must be enabled in order to enable profiling.])
	fi
fi

AC_SUBST(SEED_DEBUG_CFLAGS)

dnl ===========================Darwin Build====================================
SEED_OSX_CFLAGS=
SEED_OSX_LDFLAGS=

case "$host_os" in
	darwin*)
		SEED_OSX_CFLAGS="-F/Applications/WebKit.app/Contents/Frameworks/10.5 -framework JavaScriptCore"
		SEED_OSX_LDFLAGS="-F/Applications/WebKit.app/Contents/Frameworks/10.5 -framework JavaScriptCore"
		;;
	*)
		PKG_CHECK_MODULES(WEBKIT, webkit-1.0)
		AC_SUBST(WEBKIT_CFLAGS)
		AC_SUBST(WEBKIT_LDFLAGS)
	;;


esac
AC_SUBST(SEED_OSX_CFLAGS)
AC_SUBST(SEED_OSX_LDFLAGS)

AC_CHECK_HEADERS(pty.h)

dnl =============================gtk-doc=======================================
GTK_DOC_CHECK(1.9)

dnl =============================Output========================================
SHAVE_INIT([scripts],[enable])
AC_CONFIG_FILES([
seed.pc
Makefile

doc/conventions.html
doc/runtime.html
doc/tutorial-standalone/tutorial.html

doc/modules/canvas/canvas.html
doc/modules/multiprocessing/multiprocessing.html

doc/modules/canvas/Makefile
doc/modules/multiprocessing/Makefile
doc/modules/readline/Makefile
doc/modules/gtkbuilder/Makefile
doc/modules/sqlite/Makefile
doc/modules/sandbox/Makefile

doc/mapping/mapping.html

scripts/shave
scripts/shave-libtool

libseed/Makefile
po/Makefile.in
src/Makefile
extensions/Seed.js
extensions/Makefile
tests/Makefile
tests/javascript/Makefile
tests/javascript/gtypes/Makefile
tests/javascript/signals/Makefile
tests/javascript/structs/Makefile
tests/c/Makefile
doc/Makefile
doc/modules/Makefile
doc/tutorial-standalone/Makefile
doc/reference/Makefile
doc/mapping/Makefile
examples/Makefile
examples/clutter-pad/Makefile
examples/clutter-pad/examples/Makefile
examples/clutter-shader/Makefile
examples/glib/Makefile
examples/browser/Makefile
examples/turtle/Makefile
examples/opengl/Makefile
examples/twitter/Makefile
examples/dbus/Makefile
examples/xml/Makefile
modules/Makefile
modules/example/Makefile
modules/sqlite/Makefile
modules/canvas/Makefile
modules/readline/Makefile
modules/multiprocessing/Makefile
modules/sandbox/Makefile
modules/os/Makefile
modules/dbus/Makefile
modules/dbus/util/Makefile
modules/libxml/Makefile
modules/cairo/Makefile
modules/gtkbuilder/Makefile
modules/gettext/Makefile
modules/mpfr/Makefile
modules/ffi/Makefile

libseed/seed-path.h
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
   DBus.......................$want_dbus_module
   libxml.....................$want_libxml_module
   cairo......................$want_cairo_module
   gtkbuilder.................$want_gtkbuilder_module
   gettext....................$want_gettext_module
   mpfr.......................$want_mpfr_module
   ffi........................$want_ffi_module

C Examples:
   Turtle.....................$want_turtle_example
"

if test "x$want_readline_module" != "xyes"; then
echo "WARNING: The readline module is not enabled.
         Without this, the Seed REPL will not work;
         however, most Seed applications will still run.
"
fi
