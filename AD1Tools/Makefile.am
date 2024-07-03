ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
    libad1


bin_PROGRAMS = \
    ad1info \
    ad1verify \
    ad1extract \
    ad1mount 


ad1info_SOURCES = \
    ad1info.c ad1info.h

ad1info_LDADD = \
    libad1/libad1.la


ad1verify_SOURCES = \
    ad1verify.c ad1verify.h

ad1verify_LDADD = \
    libad1/libad1.la


ad1extract_SOURCES = \
    ad1extract.c ad1extract.h

ad1extract_LDADD = \
    libad1/libad1.la


ad1mount_SOURCES = \
    ad1mount.c ad1mount.h

ad1mount_LDADD = \
    libad1/libad1.la