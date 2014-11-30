AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AM_CPPFLAGS = \
-I$(top_srcdir)/src \
-I$(top_srcdir)/../external/ezOptionParser-0.2.1/ \
-I$(top_srcdir)/../external/lua-5.2.2/src

LDADD = $(top_srcdir)/../external/lua-5.2.2/src/liblua.a $(DL_LIB) -ldl

bin_PROGRAMS = metabuilder
metabuilder_SOURCES = \
	src/block.cpp \
	src/core.h \
	src/platform \
	src/writer.cpp \
	src/block.h \
	src/main.cpp \
	src/platformparam.cpp \
	src/writer.h \
	src/common.cpp \
	src/makesetup.cpp \
	src/platformparam.h \
	src/writer_msvc.cpp \
	src/common.h \
	src/makesetup.h \
	src/solution.cpp \
	src/writer_msvc.h \
	src/configparam.cpp \
	src/metabase.cpp \
	src/solution.h \
	src/writer_xcode.cpp \
	src/configparam.h \
	src/metabase.h \
	src/target.cpp \
    src/writer_xcode.h \
	src/core.cpp \
	src/metabuilder_pch.h \
	src/target.h \
	src/platform/platform.cpp \
	src/platform/POSIX/platform_posix.cpp 
	src/platform/Windows/platform_windows.cpp 
	
dist_noinst_SCRIPTS = autogen.sh


