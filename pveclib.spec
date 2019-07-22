Name:    pveclib
Version: 1.0.3
Release: 1%{?dist}
Summary: Library for simplified access to PowerISA vector operations
License: ASL 2.0
URL:     https://github.com/open-power-sdk/pveclib
Source0: https://github.com/open-power-sdk/pveclib/archive/v%{version}.tar.gz

ExclusiveArch: ppc %{power64}
BuildRequires: libtool autoconf-archive gcc-c++

%description
A library of useful vector operations for PowerISA 2.06 or later. Pveclib
builds on the PPC vector built-ins provided by <altivec.h> to provide higher
level operations. These operations also bridge gaps in compiler builtin
support for the latest PowerISA and functional differences between versions
of the PowerISA. The intent is to improve the productivity of application
developers who need to optimize their applications or dependent libraries for
POWER.

%package devel
Summary: Header files for pveclib
Requires: %{name}%{?_isa} = %{version}-%{release}
%description devel
Contains header files for using pveclib operations as inline vector
instructions.

%package static
Summary:  This package contains static libraries for %{name}
Requires: %{name}%{?_isa} = %{version}-%{release}
%description static
This package contains static libraries for pveclib.
So far only constant vectors used in conversions.

%prep
%autosetup

%build
%configure --docdir=%{_docdir}/%{name}
%make_build

%install
%make_install

%check
# do not fail on test failures as builder might not support all required features
make check || :

# we are installing it using doc
find %{buildroot} -type f -name "*.la" -delete

%files
%license LICENSE COPYING
%doc COPYING README.md CONTRIBUTING.md ChangeLog.md
%{_libdir}/libpvec.so.*

%files devel
%doc README.md
%{_libdir}/libpvec.so
%{_includedir}/pveclib

%files static
%doc README.md
%{_libdir}/libpvec.a

%changelog
* Fri July 22 2019 Munroe S <munroesj52@gmail.com> 1.0.3-1
- Updates for RPM release.
* Fri May 31 2019 Munroe S <munroesj52@gmail.com> 1.0.2-1
- Initial RPM release
