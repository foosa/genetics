AC_INIT([genetics], 1.0)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_RANLIB
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11

dnl For unit tests
PKG_CHECK_MODULES([gtest], [gtest]) 

dnl GSL
AC_CHECK_LIB([m], [cos])
AC_CHECK_LIB([gslcblas], [cblas_dgemm])
AC_CHECK_LIB([gsl], [gsl_blas_dgemm])

dnl Boost
AX_BOOST_BASE
AX_BOOST_SERIALIZATION
AX_BOOST_PROGRAM_OPTIONS

AC_CONFIG_FILES([
	Makefile	 			\
	src/Makefile 			\
	src/cluster/Makefile	\
	src/evolution/Makefile	\
	test/Makefile
])

AC_OUTPUT
