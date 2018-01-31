AC_INIT([genetics], 1.0)
AM_INIT_AUTOMAKE()
AC_PROG_RANLIB
AC_PROG_CXX

dnl For unit tests
PKG_CHECK_MODULES([gtest], [gtest]) 

dnl GSL
AC_CHECK_LIB([m], [cos])
AC_CHECK_LIB([gslcblas], [cblas_dgemm])
AC_CHECK_LIB([gsl], [gsl_blas_dgemm])


AC_CONFIG_FILES([
	Makefile	 			\
	src/Makefile 			\
	src/cluster/Makefile	\
	src/evolution/Makefile	\
	test/Makefile
])

AC_OUTPUT
