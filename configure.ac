#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([MineDraft],
        m4_esyscmd([git describe | tr -d '\n']),
        [ole.chr@microman.no])
AC_CONFIG_SRCDIR([src/level.cpp])
AC_CONFIG_HEADERS([config.h])

# Automake init
AM_INIT_AUTOMAKE([foreign dist-bzip2 dist-lzma])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([png], [png_write_row], [],
             AC_MSG_ERROR([This package requires libpng.]))
AC_CHECK_LIB([z], [gzopen], [],
             AC_MSG_ERROR([This package requires zlib.]))

# Checks for header files.
AC_CHECK_HEADERS_ONCE([unistd.h windows.h], [break])

# Check for OpenMP support.
AC_OPENMP

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
