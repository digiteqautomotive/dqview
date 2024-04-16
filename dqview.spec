Name:           dqview
Version:        4.0
Release:        1
Summary:        Digiteq Automotive video player
License:        GPL-3.0-only
Group:          Productivity/Multimedia/Video/Players
Url:            http://www.digiteqautomotive.com
Source0:        dqview.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  make
BuildRequires:  qt6-core-devel
BuildRequires:  qt6-gui-devel
BuildRequires:  qt6-widgets-devel
BuildRequires:  vlc-devel


%description
DQ Viewer is a video player designed to play streams from all Digiteq Automotive
frame grabber devices.

%prep
%setup -q -n dqview

%build
%{qmake6} dqview.pro
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
* Tue Apr 16 18:06:32 CEST 2024 - martin.tuma@digiteqautomotive.com 4.0-1
- Added support for video output playback (Linux only).
- Added copying output parameters from input device parameters.
- Added support for Qt6.

* Fri May 12 12:47:43 CEST 2023 - martin.tuma@digiteqautomotive.com 3.0-1
- System-wide installer (Windows only).
- Use DirectShow cameras specific aspect radio fix (Windows only).
- Added rotate-90 and rotate-270 video transformation options.
- Added detected video signal resolution info.

* Thu Feb 23 12:34:47 CET 2023 - martin.tuma@digiteqautomotive.com 2.7-1
- Added missing udev rules.
- Distinguish card instances in device menus (Windows only).

* Thu Apr 21 12:49:29 CEST 2022 - martin.tuma@digiteqautomotive.com 2.6-1
- Output PCLK frequency can now be set up to 195000kHz.
- Fixed output VBack/Front porch configuration.

* Thu Mar 31 12:41:23 CEST 2022 - martin.tuma@digiteqautomotive.com 2.5-1
- Fixed VSync configuration logic.
- Added aspect ratio detection/automatic setup (Windows only).

* Thu Feb 10 12:42:23 CET 2022 - martin.tuma@digiteqautomotive.com 2.4-1
- Added VFronPotch, VBackPorch and VSyncWidth configuration parameters.

* Thu Jan 20 13:18:17 CET 2022 - martin.tuma@digiteqautomotive.com 2.3-1
- Added some more signal configuration parameters (porch, HSync width, PCLK).
- Fixed property write error checking.

* Thu Sep 23 14:06:28 CEST 2021 - martin.tuma@digiteqautomotive.com 2.2-1
- Added output devices configuration.

* Wed Aug 18 11:07:11 CEST 2021 - martin.tuma@digiteqautomotive.com 2.1-1
- Added missing module ID and card serial number properties on Windows.

* Thu May 20 08:59:57 CEST 2021 - martin.tuma@digiteqautomotive.com 2.0-1
- Added MGB4 device configuration.
- RPM package.
