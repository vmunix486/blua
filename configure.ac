dnl configure.ac for blua (Lua 5.5 + raylib + inih)
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([blua], [5.5.0], [], [blua])
AC_CONFIG_SRCDIR([src/lua.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])

dnl Clear default CFLAGS set by automake; we build our own below.
: ${CFLAGS=""}

dnl Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR

dnl Check for C99 support (Lua requires it)
AX_CHECK_COMPILE_FLAG([-std=gnu99], [CFLAGS="$CFLAGS -std=gnu99"])

dnl ==========================================================================
dnl Compiler optimization flag detection
dnl These are optional flags that may not be available on older compilers.
dnl Start with clean CFLAGS and build up from scratch.
dnl ==========================================================================
: ${CFLAGS=""}
CFLAGS=$(echo "$CFLAGS" | sed 's/-g//;s/-O[0-9]*//g;s/^ *//;s/ *$//')

dnl ==========================================================================
dnl Compiler optimization flag detection
dnl These are optional flags that may not be available on older compilers.
dnl ==========================================================================

dnl Check for -Ofast (GCC 4.6+)
AC_MSG_CHECKING([whether to use -Ofast])
AC_ARG_ENABLE([ofast],
  [AS_HELP_STRING([--enable-ofast], [use -Ofast optimization @<:@default=auto@:>@])],
  [enable_ofast=$enableval],
  [enable_ofast=auto])
if test "x$enable_ofast" = "xyes" -o "x$enable_ofast" = "xauto"; then
  AX_CHECK_COMPILE_FLAG([-Ofast], [
    if test "x$enable_ofast" = "xyes"; then
      CFLAGS="$CFLAGS -Ofast"
      AC_MSG_RESULT([yes])
    else
      CFLAGS="$CFLAGS -Ofast"
      AC_MSG_RESULT([yes (auto)])
    fi
  ], [
    if test "x$enable_ofast" = "xyes"; then
      AC_MSG_ERROR([-Ofast is not supported by this compiler])
    fi
    AC_MSG_RESULT([no (not supported)])
  ])
else
  AC_MSG_RESULT([no (disabled)])
fi

dnl Check for -flto (GCC 4.5+, Clang 3.0+)
AC_MSG_CHECKING([whether to use -flto])
AC_ARG_ENABLE([lto],
  [AS_HELP_STRING([--enable-lto], [use link-time optimization @<:@default=auto@:>@])],
  [enable_lto=$enableval],
  [enable_lto=auto])
if test "x$enable_lto" = "xyes" -o "x$enable_lto" = "xauto"; then
  AX_CHECK_COMPILE_FLAG([-flto], [
    AX_CHECK_LINK_FLAG([-flto], [
      if test "x$enable_lto" = "xyes"; then
        CFLAGS="$CFLAGS -flto"
        LDFLAGS="$LDFLAGS -flto"
        AC_MSG_RESULT([yes])
      else
        CFLAGS="$CFLAGS -flto"
        LDFLAGS="$LDFLAGS -flto"
        AC_MSG_RESULT([yes (auto)])
      fi
    ], [
      if test "x$enable_lto" = "xyes"; then
        AC_MSG_ERROR([-flto is not supported by the linker])
      fi
      AC_MSG_RESULT([no (linker does not support it)])
    ])
  ], [
    if test "x$enable_lto" = "xyes"; then
      AC_MSG_ERROR([-flto is not supported by this compiler])
    fi
    AC_MSG_RESULT([no (not supported)])
  ])
else
  AC_MSG_RESULT([no (disabled)])
fi

dnl Check for -mtune (GCC supports this on all platforms)
dnl On x86, -march=native is more common; on ARM, -mtune=native works.
dnl We try -march first, then fall back to -mtune.
AC_MSG_CHECKING([for CPU optimization flag])
AC_ARG_WITH([cpu],
  [AS_HELP_STRING([--with-cpu=MODEL],
    [set CPU model for -march or -mtune @<:@default=native@:>@])],
  [cpu_model=$withval],
  [cpu_model=native])

cpu_flag=""
if test "x$cpu_model" != "xno"; then
  AX_CHECK_COMPILE_FLAG([-march=$cpu_model], [cpu_flag="-march=$cpu_model"], [
    AX_CHECK_COMPILE_FLAG([-mtune=$cpu_model], [cpu_flag="-mtune=$cpu_model"])
  ])
fi

if test -n "$cpu_flag"; then
  AC_MSG_RESULT([$cpu_flag])
  CFLAGS="$CFLAGS $cpu_flag"
else
  AC_MSG_RESULT([none available])
fi

dnl Only add -O3 as baseline if no -O flag was already added
case "$CFLAGS" in
  *-O*) ;;
  *) AX_CHECK_COMPILE_FLAG([-O3], [CFLAGS="$CFLAGS -O3"]) ;;
esac

dnl Check for common warning flags
AX_CHECK_COMPILE_FLAG([-Wall], [CFLAGS="$CFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra], [CFLAGS="$CFLAGS -Wextra"])

dnl Checks for libraries.
AC_CHECK_LIB([m], [sin])
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([rt], [clock_gettime])
AC_CHECK_LIB([pthread], [pthread_create])

dnl Check for raylib
AC_ARG_ENABLE([raylib],
  [AS_HELP_STRING([--disable-raylib], [disable raylib support])],
  [enable_raylib=$enable_raylib],
  [enable_raylib=yes])

if test "x$enable_raylib" = "xyes"; then
  AC_CHECK_LIB([raylib], [InitWindow], [
    LIBS="$LIBS -lraylib"
    AC_CHECK_LIB([glfw], [glfwInit], [LIBS="$LIBS -lglfw"])
    AC_CHECK_LIB([GL], [glBegin], [LIBS="$LIBS -lGL"])
    AC_CHECK_LIB([X11], [XOpenDisplay], [LIBS="$LIBS -lX11"])
    AC_DEFINE([_RAYLIB], [1], [Enable raylib support])
  ], [
    AC_MSG_WARN([raylib not found, building without raylib support])
    enable_raylib=no
  ])
fi

AM_CONDITIONAL([HAVE_RAYLIB], [test "x$enable_raylib" = "xyes"])

dnl Output files.
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

dnl Print summary
echo ""
echo "blua $PACKAGE_VERSION configuration summary:"
echo "  C compiler:       $CC"
echo "  CFLAGS:           $CFLAGS"
echo "  LDFLAGS:          $LDFLAGS"
echo "  LIBS:             $LIBS"
echo "  raylib support:   $enable_raylib"
echo ""
