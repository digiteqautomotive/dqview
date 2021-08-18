Name:           dqview
Version:        2.1
Release:        1
Summary:        Digiteq Automotive video player
License:        GPL-3.0-only
Group:          Productivity/Multimedia/Video/Players
Url:            http://www.digiteqautomotive.com
Source0:        dqview.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  make
BuildRequires:  libQt5Core-devel
BuildRequires:  libQt5Gui-devel
BuildRequires:  libQt5Widgets-devel
BuildRequires:  vlc-devel


%description
DQ Viewer is a video player designed to play streams from all Digiteq Automotive
frame grabber devices.

%prep
%setup -q -n dqview

%build
%{qmake5} dqview.pro
make %{?_smp_mflags}

%install
install -d 755 %{buildroot}/%{_bindir}
install -d 755 %{buildroot}/%{_datadir}/applications
install -d 755 %{buildroot}/%{_datadir}/icons/hicolor/64x64/apps
install -d 755 %{buildroot}/%{_sysconfdir}/udev/rules.d
install -m 755 dqview %{buildroot}/%{_bindir}/%{name}
install -m 644 icons/app.png %{buildroot}/%{_datadir}/icons/hicolor/64x64/apps/%{name}.png
install -m 644 dqview.desktop %{buildroot}/%{_datadir}/applications/%{name}.desktop
install -m 644 udev/99-mgb4.rules %{buildroot}/%{_sysconfdir}/udev/rules.d/99-mgb4.rules

%post
if [ -x /usr/bin/update-desktop-database ]; then
	/usr/bin/update-desktop-database > /dev/null || :
fi

%postun
if [ -x /usr/bin/update-desktop-database ]; then
	/usr/bin/update-desktop-database > /dev/null || :
fi

%files
%defattr(-,root,root)
%{_bindir}/*
%{_datadir}/applications/*
%{_datadir}/icons/hicolor/*
%{_sysconfdir}/udev/rules.d/99-mgb4.rules

%changelog
* Wed Aug 18 11:07:11 CEST 2021 - martin.tuma@digiteqautomotive.com 2.1-1
- Added missing module ID and card serial number properties on Windows

* Thu May 20 08:59:57 CEST 2021 - martin.tuma@digiteqautomotive.com 2.0-1
- Added MGB4 device configuration.
- RPM package
