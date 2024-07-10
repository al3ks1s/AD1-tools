ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libad1.la

libad1_la_SOURCES = \
    libad1_definitions.h \
    libad1_extract.c libad1_extract.h \
    libad1_file_reader.c libad1_file_reader.h \
    libad1_fuse.c libad1_fuse.h \
    libad1_hash.c libad1_hash.h \
    libad1_encryption.c libad1_encryption.h \
    libad1_printer.c libad1_printer.h \
    libad1_reader.c libad1_reader.h \
    libad1_session.c libad1_session.h \
    libad1_tree.c libad1_tree.h

libad1_la_LIBADD = \
    @OPENSSL_LIBS@ \
    -lfuse
