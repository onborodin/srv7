AC_INIT(main.cpp,0.01)

AC_CONFIG_MACRO_DIR(m4)

AM_INIT_AUTOMAKE
AC_PREFIX_DEFAULT(/usr/local)
AC_LANG([C++])

AC_PROG_CXX(clang++ g++ c++)
AC_PROG_CXX_C_O

dnl AC_PATH_TOOL(BISON, bison)
dnl AC_PATH_TOOL(REFLEX, reflex)
dnl AC_CHECK_LIB(reflex, reflex::convert)

AC_HEADER_STDBOOL

AM_CONFIG_HEADER([autoconf.hpp])

AX_CXX_HAVE_STD
AX_CXX_HAVE_SSTREAM
#AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
#AX_CXX_COMPILE_STDCXX_14([noext],[mandatory])
AX_CXX_COMPILE_STDCXX_17([noext],[mandatory])

AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([syslog.h])

AC_FUNC_FORK

AC_TYPE_PID_T
AC_TYPE_UID_T
AC_TYPE_SIZE_T

AC_OUTPUT([
Makefile
])

dnl EOF
