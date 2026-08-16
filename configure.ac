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
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR

dnl Check for C standard: --enable-c89 for strict C89 mode (default: C99)
dnl Note: C89 mode is incompatible with POSIX features (LUA_USE_POSIX).
dnl On Linux, C99 is required for POSIX headers (signal, etc.).
AC_ARG_ENABLE([c89],
  [AS_HELP_STRING([--enable-c89], [compile with strict C89 (ANSI C) instead of C99 @<:@default=no@:>@])],
  [enable_c89=$enableval],
  [enable_c89=no])

if test "x$enable_c89" = "xyes"; then
  dnl C89 mode on Linux/POSIX is not supported by Lua 5.5.
  dnl Only use it for non-POSIX builds (e.g. Windows cross-compilation).
  AC_MSG_WARN([C89 mode is incompatible with POSIX; some Lua features may not work on Linux])
  AX_CHECK_COMPILE_FLAG([-std=c89], [
    CFLAGS="$CFLAGS -std=c89"
    AC_DEFINE([LUA_USE_C89], [1], [Compile in strict C89 mode])
  ], [
    AX_CHECK_COMPILE_FLAG([-ansi], [
      CFLAGS="$CFLAGS -ansi"
      AC_DEFINE([LUA_USE_C89], [1], [Compile in strict C89 mode])
    ], [
      AC_MSG_ERROR([C89/ANSI mode requested but compiler does not support -std=c89 or -ansi])
    ])
  ])
else
  AX_CHECK_COMPILE_FLAG([-std=gnu99], [CFLAGS="$CFLAGS -std=gnu99"])
fi

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
AX_CHECK_COMPILE_FLAG([-Wno-stringop-overflow], [CFLAGS="$CFLAGS -Wno-stringop-overflow"])

dnl Checks for libraries.
AC_CHECK_LIB([m], [sin])
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([rt], [clock_gettime])
AC_CHECK_LIB([pthread], [pthread_create])

dnl Define Linux mode for Lua (LUA_USE_LINUX implies LUA_USE_POSIX in luaconf.h).
AC_DEFINE([LUA_USE_LINUX], [1], [Enable Linux-specific Lua features])

dnl Check for raylib
AC_ARG_ENABLE([raylib],
  [AS_HELP_STRING([--disable-raylib], [disable raylib support])],
  [enable_raylib=$enableval],
  [enable_raylib=yes])

AC_ARG_WITH([raylib],
  [AS_HELP_STRING([--with-raylib=DIR], [path to raylib installation @<:@default=auto@:>@])],
  [raylib_prefix=$withval],
  [raylib_prefix=""])

AC_ARG_ENABLE([raylib-kms],
  [AS_HELP_STRING([--enable-raylib-kms], [use DRM/KMS backend instead of desktop GL @<:@default=no@:>@])],
  [enable_raylib_kms=$enableval],
  [enable_raylib_kms=no])

if test "x$enable_raylib" = "xyes"; then
  dnl Save original CFLAGS/LIBS/LDFLAGS
  raylib_save_CFLAGS="$CFLAGS"
  raylib_save_LIBS="$LIBS"
  raylib_save_LDFLAGS="$LDFLAGS"

  dnl Build candidate flags for raylib search
  raylib_CFLAGS=""
  raylib_LDFLAGS=""

  dnl If a path was given, add it to search paths
  if test -n "$raylib_prefix"; then
    raylib_CFLAGS="-I$raylib_prefix/include"
    raylib_LDFLAGS="-L$raylib_prefix/lib -L$raylib_prefix/lib64"
  fi

  dnl Also try common locations (skip X11 paths for KMS mode)
  if test "x$enable_raylib_kms" = "xno"; then
    _raylib_search_dirs="/usr/local /usr /usr/X11R7"
  else
    _raylib_search_dirs="/usr/local /usr"
  fi
  for _libdir in $_raylib_search_dirs; do
    if test "x$raylib_prefix" != "x$_libdir"; then
      if test -d "$_libdir/include" -a -z "$(echo $raylib_CFLAGS | grep $_libdir/include)"; then
        raylib_CFLAGS="$raylib_CFLAGS -I$_libdir/include"
      fi
      if test -d "$_libdir/lib64"; then
        raylib_LDFLAGS="$raylib_LDFLAGS -L$_libdir/lib64"
      fi
      if test -d "$_libdir/lib"; then
        raylib_LDFLAGS="$raylib_LDFLAGS -L$_libdir/lib"
      fi
    fi
  done

  dnl Temporarily add raylib flags for the check
  CFLAGS="$CFLAGS $raylib_CFLAGS"
  LDFLAGS="$LDFLAGS $raylib_LDFLAGS"

  if test "x$enable_raylib_kms" = "xyes"; then
    dnl DRM/KMS backend: -lraylib -lGLESv2 -lEGL -ldrm -lgbm -lpthread -lrt -lm -ldl -latomic
    raylib_test_LIBS="-lraylib -lGLESv2 -lEGL -ldrm -lgbm -lpthread -lrt -lm -ldl -latomic"
  else
    dnl Desktop GL backend: -lraylib -lm -lpthread -lrt -ldl -lX11
    raylib_test_LIBS="-lraylib -lm -lpthread -lrt -ldl -lX11"
  fi
  LIBS="$raylib_test_LIBS $LIBS"
  AC_CHECK_LIB([raylib], [InitWindow], [
    if test "x$enable_raylib_kms" = "xyes"; then
      LIBS="-lraylib -lGLESv2 -lEGL -ldrm -lgbm -lpthread -lrt -lm -ldl -latomic"
    else
      LIBS="-lraylib -lm -lpthread -lrt -ldl -lX11"
      AC_CHECK_LIB([glfw], [glfwInit], [LIBS="$LIBS -lglfw"], [], [-lraylib -lm -lpthread -lrt -ldl -lX11])
      AC_CHECK_LIB([GL], [glBegin], [LIBS="$LIBS -lGL"], [], [-lraylib -lm -lpthread -lrt -ldl -lX11 -lglfw])
    fi
    CFLAGS="$raylib_save_CFLAGS $raylib_CFLAGS"
    LDFLAGS="$raylib_save_LDFLAGS $raylib_LDFLAGS"
    AC_DEFINE([_RAYLIB], [1], [Enable raylib support])
    if test "x$enable_raylib_kms" = "xyes"; then
      AC_DEFINE([PLATFORM_DRM], [1], [Use DRM/KMS platform for raylib])
    fi
    if test -n "$raylib_prefix"; then
      AC_MSG_RESULT([using raylib from $raylib_prefix])
    fi
  ], [
    AC_MSG_WARN([raylib not found, building without raylib support])
    enable_raylib=no
    CFLAGS="$raylib_save_CFLAGS"
    LIBS="$raylib_save_LIBS"
    LDFLAGS="$raylib_save_LDFLAGS"
  ])
fi

AM_CONDITIONAL([HAVE_RAYLIB], [test "x$enable_raylib" = "xyes"])

dnl Check for FLTK
AC_ARG_ENABLE([fltk],
  [AS_HELP_STRING([--disable-fltk], [disable FLTK support @<:@default=yes@:>@])],
  [enable_fltk=$enableval],
  [enable_fltk=yes])

AC_ARG_WITH([fltk],
  [AS_HELP_STRING([--with-fltk=PATH], [path to fltk-config @<:@default=auto@:>@])],
  [fltk_config=$withval],
  [fltk_config=""])

if test "x$enable_fltk" = "xyes"; then
  dnl Find fltk-config
  if test -n "$fltk_config"; then
    FLTK_CONFIG="$fltk_config"
  else
    AC_PATH_PROG([FLTK_CONFIG], [fltk-config], [no])
  fi

  if test "x$FLTK_CONFIG" = "xno"; then
    AC_MSG_ERROR([fltk-config not found. Install FLTK development files or use --with-fltk=PATH])
  fi

  AC_MSG_CHECKING([FLTK version])
  FLTK_VERSION=$($FLTK_CONFIG --version)
  AC_MSG_RESULT([$FLTK_VERSION])

  dnl Get flags from fltk-config
  FLTK_CXXFLAGS=$($FLTK_CONFIG --cxxflags)
  FLTK_LIBS=$($FLTK_CONFIG --ldflags)

  AC_SUBST([FLTK_CXXFLAGS])
  AC_SUBST([FLTK_LIBS])
  AC_DEFINE([_FLTK], [1], [Enable FLTK support])
fi

AM_CONDITIONAL([HAVE_FLTK], [test "x$enable_fltk" = "xyes"])

dnl Check for ncurses
AC_ARG_ENABLE([curses],
  [AS_HELP_STRING([--disable-curses], [disable ncurses support @<:@default=yes@:>@])],
  [enable_curses=$enableval],
  [enable_curses=yes])

if test "x$enable_curses" = "xyes"; then
  AC_CHECK_LIB([ncurses], [initscr], [
    AC_DEFINE([_NCURSES], [1], [Enable ncurses support])
    AC_CHECK_HEADERS([ncurses.h])
  ], [
    AC_MSG_WARN([ncurses not found, building without ncurses support])
    enable_curses=no
  ])
fi

AM_CONDITIONAL([HAVE_NCURSES], [test "x$enable_curses" = "xyes"])

dnl Output files.
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

dnl Print summary
if test "x$enable_raylib" = "xyes"; then
  if test "x$enable_raylib_kms" = "xyes"; then
    raylib_backend="yes (DRM/KMS)"
  else
    raylib_backend="yes (Desktop GL)"
  fi
else
  raylib_backend="no"
fi

echo ""
echo "blua $PACKAGE_VERSION configuration summary:"
echo "  C compiler:       $CC"
if test "x$enable_c89" = "xyes"; then
  echo "  C standard:       C89 (ANSI C) -- warning: incompatible with POSIX"
else
  echo "  C standard:       C99"
fi
echo "  CFLAGS:           $CFLAGS"
echo "  LDFLAGS:          $LDFLAGS"
echo "  LIBS:             $LIBS"
echo "  raylib support:   $raylib_backend"
if test "x$enable_fltk" = "xyes"; then
  echo "  FLTK support:     yes (v$FLTK_VERSION)"
  echo "  FLTK_CXXFLAGS:    $FLTK_CXXFLAGS"
  echo "  FLTK_LIBS:        $FLTK_LIBS"
else
  echo "  FLTK support:     no"
fi
if test "x$enable_curses" = "xyes"; then
  echo "  ncurses support:  yes"
else
  echo "  ncurses support:  no"
fi
echo ""
