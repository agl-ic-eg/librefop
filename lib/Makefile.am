AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

lib_LTLIBRARIES = librefop.la

librefop_la_SOURCES = \
	fileop.c file-util.c \
	static-configurator.c \
	libredundancyfileop.c 

librefop_la_LBSADD = 

librefop_la_CFLAGS = \
	-g \
	-I$(top_srcdir)/include \
	-D_GNU_SOURCE 

librefop_la_LDFLAGS = \
	-export-symbols libredundancyfileop.sym -version-info 0:0:0

# configure option 
if ENABLE_ADDRESS_SANITIZER
CFLAGS   += -fsanitize=address
endif

if ENABLE_GCOV
CFLAGS   += -coverage
endif

CLEANFILES = *.gcda *.gcno
