AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

lib_LTLIBRARIES = libredundancyfileop.la

libredundancyfileop_la_SOURCES = \
	fileop.c file-util.c \
	static-configurator.c \
	libredundancyfileop.c 

libredundancyfileop_la_LBSADD = 

libredundancyfileop_la_CFLAGS = \
	-g \
	-I$(top_srcdir)/include \
	-D_GNU_SOURCE 

libredundancyfileop_la_LDFLAGS = \
	-export-symbols libredundancyfileop.sym -version-info 0:0:0

# configure option 
if ENABLE_ADDRESS_SANITIZER
CFLAGS   += -fsanitize=address
endif

if ENABLE_GCOV
CFLAGS   += -coverage
endif

CLEANFILES = *.gcda *.gcno
