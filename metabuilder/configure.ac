AC_INIT([metabuilder], [1.0])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign])

AC_CANONICAL_HOST
# Check for which host we are on and setup a few things
# specifically based on the host
case $host_os in
  darwin* )
        # Do something specific for mac
        ;;
  linux*)
        # Do something specific for linux
        AC_ARG_WITH([-ldl], [DL_LIB="-ldl"])
        AC_SUBST([DL_LIB])
        ;;
   *BSD*)
        # Do something specific for BSD
        ;;
    *)
        #Default Case
        AC_MSG_ERROR([Your platform is not currently supported])
        ;;
esac

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

