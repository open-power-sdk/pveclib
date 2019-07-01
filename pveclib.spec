Name:    pveclib	
Version: 1.0.2
Release: 7%{?dist}
Summary: Library for simplified access to PowerISA vector operations
License: APACHE
URL:     https://github.com/open-power-sdk/pveclib
Source0: https://github.com/open-power-sdk/pveclib/archive/v%{version}.tar.gz

ExclusiveArch: ppc %{power64}
BuildRequires: libtool autoconf-archive
	
 
	
%package devel
Summary: Header files for pveclib
Requires: %{name}%{?_isa} = %{version}-%{release}
%description devel
Contains header files for using pveclib operations as inline vector
instructions.
	
%description
A library of useful vector operations for PowerISA 2.06 or later. Pveclib
builds on the PPC vector intrinsics provided by <altivec.h> to provide higher
level operations. These operations also bridge gaps in compiler builtin
support for the latest PowerISA and functional differences between versions
of the PowerISA. The intent is to improve the productivity of application
developers who need to optimize their applications or dependent libraries for
POWER.

%prep
%setup -q -n %{name}-%{version}

%build
%configure --docdir=%{_docdir}/%{name}
make %{?_smp_mflags}

%check

%configure
	
# do not fail on test failures as builder might not support all required features
make check || :

%install
make install DESTDIR=$RPM_BUILD_ROOT
	
# we are installing it using doc
rm -rf %{buildroot}/usr/share/doc/libpvec/ChangeLog.md
find %{buildroot} -type f -name "*.la" -delete

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%{!?_licensedir:%global license %%doc}
%license COPYING
%doc README.md CONTRIBUTING.md ChangeLog.md
%{_libdir}/libpvec.so.*

%files devel
%{_libdir}/libpvec.so
%{_libdir}/libpvec.a
%dir %{_includedir}/pveclib
%{_includedir}/pveclib/vec_common_ppc.h
%{_includedir}/pveclib/vec_f128_ppc.h
%{_includedir}/pveclib/vec_f64_ppc.h
%{_includedir}/pveclib/vec_f32_ppc.h
%{_includedir}/pveclib/vec_int128_ppc.h
%{_includedir}/pveclib/vec_int64_ppc.h
%{_includedir}/pveclib/vec_int32_ppc.h
%{_includedir}/pveclib/vec_int16_ppc.h
%{_includedir}/pveclib/vec_char_ppc.h
%{_includedir}/pveclib/vec_bcd_ppc.h
	
%changelog
* Fri May 31 2019 Munroe S <munroesj52@gmail.com> 1.0.2
- Initial RPM release
