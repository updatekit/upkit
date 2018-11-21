dnl Process this file with autoconf to produce a configure script.
AC_INIT(libpull, 0.0.1, antonio@langiu.com)
AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])
AM_MAINTAINER_MODE([disable])

dnl Config header
dnl I am exporting the configuration files in the header since it
dnl modifies the API of the library. It does not modify the functions
dnl but it specifies the implementation of some struct and unions and
dnl thus when the library is compiled in this way it relies on these
dnl structures.
AC_CONFIG_HEADERS([default_config.h:default_config.h.in include/libpull/common/libpull_config.h:libpull_config.h.in])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
dnl AM_PROG_AR
AC_PROG_RANLIB

dnl Recursive targets
AC_CONFIG_FILES([Makefile test/Makefile utils/firmware_tool/Makefile utils/server/Makefile])

dnl Checks for libraries.

dnl Checks for header files.
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_CHECK_FUNCS([bzero memset])

dnl Enable verbosity level (The default is VERBOSITY_INFO)
AC_ARG_ENABLE([log-error], AC_HELP_STRING([--enable-log-error], 
              [Log only errors @<:@default=no@:>@]), [verbosity=1])
AC_ARG_ENABLE([log-warn], AC_HELP_STRING([--enable-log-warn], 
              [Log errors and  warning @<:@default=no@:>@]),
              [verbosity=2])
AC_ARG_ENABLE([log-info], AC_HELP_STRING([--enable-log-info], 
              [Log error, warning and info messages @<:@default=yes@:>@]),
              [verbosity=3])
AC_ARG_ENABLE([log-debug], AC_HELP_STRING([--enable-log-debug], 
              [Log error, warning, info, and debug messages
               @<:@default=no@:>@]), [verbosity=4])
AC_ARG_ENABLE([log-verbosity], AC_HELP_STRING([--disable-log-verbosity], 
              [Disable verbosity @<:@default=no@:>@]), [verbosity=0])
AS_IF([test "x$verbosity" == "x"], [
    AC_DEFINE([LOGGER_VERBOSITY], [4],[Define logger verbosity.])
], [
    AC_DEFINE_UNQUOTED([LOGGER_VERBOSITY], [$verbosity],[Define logger verbosity.])
])


AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov],
                              [use Gcov to test the test suite])],
                              [],
                              [enable_gcov=no])
AM_CONDITIONAL([COND_GCOV],[test '!' "$enable_gcov" = no])

dnl Enable simple manifest type.
AC_ARG_ENABLE([simple-manifest], AC_HELP_STRING([--disable-simple-manifest], 
              [Disable simple manifest @<:@default=yes@:>@]), [simple-manifest=yes])
AS_IF([test "x$simple-manifest" != "xno"], [AC_DEFINE([SIMPLE_MANIFEST], [1],[Use the simple manifest.])])

dnl With cryptographic libraries.
AC_ARG_WITH([tinycrypt], AS_HELP_STRING([--with-tinycrypt],
            [Use tinycrypt as a cryptographic library]))
AS_IF([test "x$with-tinycrypt" != "xyes"], [AC_DEFINE([WITH_TINYCRYPT], [0],
      [Use tinycrypt as a cryptographic library.])])

dnl check the presence of the library from the include path or from the ext path

AC_ARG_WITH([tinydtls], AS_HELP_STRING([--with-tinydtls],
            [Use TinyDTLS as a cryptographic library]))
AS_IF([test "x$with-tinydtls" != "xyes"], [AC_DEFINE([WITH_TINYDTLS], [0],
      [Use TinyDTLS as a cryptographic library.])])

dnl check the presence of the library from the include path or from the ext path

AC_OUTPUT
