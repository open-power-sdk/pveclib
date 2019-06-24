
ACLOCAL_AMFLAGS = -I m4

#@INC_AMINCLUDE@ is recommented for automake and doxygen
#but there is an naming comflict. So we rename aminclude.am to
#doxygen.am from srcdir with allows INC_AMINCLUDE to generate 
#aminclude.am to blddir.
@INC_AMINCLUDE@
include $(top_srcdir)/doxygen.am

dist_doc_DATA = COPYING CONTRIBUTING.md README.md ChangeLog.md

SUBDIRS= src

EXTRA_DIST = $(DX_CONFIG) doc/pveclibmaindox.h

ChangeLog.md:
	$(am__cd) $(top_srcdir) && ./generate-changelog.sh \
> $(abs_top_builddir)/$@
