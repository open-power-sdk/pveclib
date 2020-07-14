
ACLOCAL_AMFLAGS = -I m4

#@INC_AMINCLUDE@ is recommented for automake and doxygen
#but there is an naming comflict. So we rename aminclude.am to
#doxygen.am from srcdir with allows INC_AMINCLUDE to generate 
#aminclude.am to blddir.
@INC_AMINCLUDE@
include $(top_srcdir)/doxygen.am

licensedir = $(datadir)/licenses/$(PACKAGE_NAME)
dist_license_DATA = LICENSE

dist_doc_DATA = COPYING CONTRIBUTING.md README.md ChangeLog.md

SUBDIRS= src

EXTRA_DIST = $(DX_CONFIG) doc/pveclibmaindox.h \
  testprograms/test_ppc_970.c \
  testprograms/test_ppc_bool_int128.c \
  testprograms/test_ppc_const_int128.c \
  testprograms/test_ppc_DFP.c \
  testprograms/test_ppc_F128arith.c \
  testprograms/test_ppc_F128.c \
  testprograms/test_ppc_F128math.c \
  testprograms/test_ppc_int128.c \
  testprograms/test_ppc_PWR6.c \
  testprograms/test_ppc_PWR7.c \
  testprograms/test_ppc_PWR8.c \
  testprograms/test_ppc_PWR9.c \
  testprograms/test_ppc_VMX.c \
  testprograms/test_ppc_VSX.c

distclean-local:
	rm $(top_builddir)/aminclude.am

ChangeLog.md:
	$(am__cd) $(top_srcdir) && ./generate-changelog.sh \
> $(abs_top_builddir)/$@
