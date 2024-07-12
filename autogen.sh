#!/bin/sh

aclocal --force -I m4 --install
autoconf --force
libtoolize --force
automake --add-missing

