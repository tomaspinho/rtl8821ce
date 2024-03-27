%global commit a3e2f7c1f91e92f2dc788e8fcd7f2986af3d19b6
%global shortcommit %(c=%{commit}; echo ${c:0:7})
%global gitdate 20230101

# buildforkernels macro hint: when you build a new version or a new release
# that contains bugfixes or other improvements then you must disable the
# "buildforkernels newest" macro for just that build; immediately after
# queuing that build enable the macro again for subsequent builds; that way
# a new akmod package will only get build when a new one is actually needed
%define buildforkernels akmod

#define prever  rc1

Name:           rtl8821ce-kmod
Version:        0.1
Release:        5%{?gitdate:.%gitdate.git%shortcommit}%{?dist}
Summary:        Kernel modules for Realtek RTL8821CE Driver
License:        GPLv2
#URL:            https://github.com/tomaspinho/rtl8821ce
URL:            https://github.com/sergiomb2/rtl8821ce
Source0:        %{url}/archive/%{commit}/rtl8821ce-%{shortcommit}.tar.gz
Source1:        rtw88_blacklist.conf
Patch0:         rename_to_rtl8821ce.patch

%global AkmodsBuildRequires %{_bindir}/kmodtool, xz, time, elfutils-libelf-devel, gcc, bc
BuildRequires:  %{AkmodsBuildRequires}

%if "%{buildforkernels}" != "akmod"
# tracks in all the kernel-devel packages
%{!?kernels:BuildRequires: buildsys-build-rpmfusion-kerneldevpkgs-%{?buildforkernels:%{buildforkernels}}%{!?buildforkernels:current}-%{_target_cpu} }
%else
%global debug_package %{nil}
%endif

# kmodtool does its magic here
%{expand:%(kmodtool --target %{_target_cpu} --repo rpmfusion --kmodname %{name} %{?buildforkernels:--%{buildforkernels}} %{?kernels:--for-kernels "%{?kernels}"} 2>/dev/null) }


%description
%{summary}.

%package -n rtl8821ce
Summary:        Common package of kernel modules for Realtek RTL8821CE Driver
Requires:       %{name} = %{version}-%{release}
Provides:       %{name}-common = %{version}-%{release}

%description -n rtl8821ce
%{summary}.

%prep
%{?kmodtool_check}
kmodtool  --target %{_target_cpu} --repo rpmfusion --kmodname %{name} %{?buildforkernels:--%{buildforkernels}} %{?kernels:--for-kernels "%{?kernels}"} 2>/dev/null

%setup -q -c
pushd rtl8821ce-%{commit}
%patch0 -p1
popd

for kernel_version  in %{?kernel_versions} ; do
cp -a rtl8821ce-%{commit} _kmod_build_${kernel_version%%___*}
done


%build
for kernel_version  in %{?kernel_versions} ; do
    pushd  _kmod_build_${kernel_version%%___*}
    make %{?_smp_mflags} \
        KSRC=${kernel_version##*___} \
        KVERS=${kernel_version%%___*} modules
    popd
done


%install
#Blacklist rtw88
install    -m 0755 -d         %{buildroot}%{_sysconfdir}/modprobe.d/
install -p -m 0644 %{SOURCE1} %{buildroot}%{_sysconfdir}/modprobe.d/

for kernel_version in %{?kernel_versions}; do
    install -d %{buildroot}%{kmodinstdir_prefix}/${kernel_version%%___*}/%{kmodinstdir_postfix}
    install _kmod_build_${kernel_version%%___*}/*.ko %{buildroot}%{kmodinstdir_prefix}/${kernel_version%%___*}/%{kmodinstdir_postfix}
done

%{?akmod_install}

%files -n rtl8821ce
%license rtl8821ce-%{commit}/COPYING
%doc rtl8821ce-%{commit}/README.md
%{_sysconfdir}/modprobe.d/rtw88_blacklist.conf

%changelog
* Wed Apr 05 2023 Sérgio Basto <sergio@serjux.com> - 0.1-5.20230101.gita3e2f7c
- Update to 20230101 6.1 kernel

* Mon Mar 22 2021 Sérgio Basto <sergio@serjux.com> - 0.1-4.20210321.git897e7c4
- 20210321 5.12-rc2

* Mon Mar 01 2021 Sérgio Basto <sergio@serjux.com> - 0.1-3.20210301.git64ac6d3
- 20210301

* Tue Oct 06 2020 Sérgio Basto <sergio@serjux.com> - 0.1-1.20201006.gitfe97b50
- Initial release

