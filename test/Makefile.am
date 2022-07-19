AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

bin_PROGRAMS = \
	interface_test interface_test_unit \
	fileop_test_utils \
	fileop_test_set_get_remove \
	fileop_test_unit \
	file_util_test

interface_test_SOURCES = \
	interface_test.cpp \
	../lib/static-configurator.c \
	../lib/file-util.c \
	../lib/fileop.c

interface_test_unit_SOURCES = \
	interface_test_unit.cpp \
	../lib/static-configurator.c \
	../lib/file-util.c \
	../lib/fileop.c

fileop_test_utils_SOURCES = \
	fileop_test_utils.cpp \
	../lib/static-configurator.c \
	../lib/file-util.c

fileop_test_set_get_remove_SOURCES = \
	fileop_test_set_get_remove.cpp \
	../lib/static-configurator.c \
	../lib/file-util.c


fileop_test_unit_SOURCES = \
	fileop_test_unit.cpp \
	../lib/static-configurator.c

file_util_test_SOURCES = \
	file_util_test.cpp

# options
# Additional library
LDADD = \
	-lrt -lpthread \
	@GTEST_MAIN_LIBS@ \
	@GMOCK_MAIN_LIBS@

interface_test_LDADD = \
	${LDADD}

# C compiler options
CFLAGS = \
	-g \
	-fpermissive \
	-fsanitize=address -coverage \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/include \
	-D_GNU_SOURCE

# C++ compiler options
CXXFLAGS = \
	-g \
	-fpermissive \
	-std=c++11 -fno-exceptions \
	-fsanitize=address -coverage \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/include \
	-D_GNU_SOURCE

# Linker options
LDFLAGS =


CLEANFILES = *.gcda *.gcno
