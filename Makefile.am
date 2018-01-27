
ACLOCAL_AMFLAGS = -I m4

#This is recommented, but does not work correcly.
#So comment it out for now.
#@INC_AMINCLUDE@

#Until we figure out why @INC_AMINCLUDE@ substitution is not working,
#Commit aminclude.am and include it directly from there.
include $(top_srcdir)/aminclude.am

dist_doc_DATA = COPYING CONTRIBUTING.md README.md

SUBDIRS= src

EXTRA_DIST = $(DX_CONFIG) doc/pveclibmaindox.h
