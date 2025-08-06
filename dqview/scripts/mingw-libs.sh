#!/bin/sh

# This script collects all mingw64 DLLs required by the dqview executable
# Run with either "qt5" or "qt6" as parameter

EXE="dqview.exe"
LIB_DIR="/mingw64/bin"
VLC_PLUGINS_DIR="/mingw64/lib/vlc/plugins"
VLC_PLUGINS="access access_output audio_filter audio_output codec d3d9 d3d11 demux mux packetizer stream_filter stream_out video_filter video_chroma video_output"
QT5_PLUGINS="platforms/qwindows.dll styles/qwindowsvistastyle.dll"
QT6_PLUGINS="platforms/qwindows.dll styles/qmodernwindowsstyle.dll"
QT_PLUGINS_DIR="/mingw64/share/$1/plugins"
OUT_DIR="."

vlc_plugins_dlls() {
	for i in $VLC_PLUGINS; do
		cd "$VLC_PLUGINS_DIR/$i"
		for j in *.dll; do
			ldd "$j" 2>&1 | grep $LIB_DIR
		done
	done | cut -d '(' -f1 | sort -u | cut -d= -f1 | tr -d '\t' | tr -d ' '
}

qt_plugins_dlls() {
	for i in $QT_PLUGINS; do
		ldd "$QT_PLUGINS_DIR/$i" 2>&1 | grep $LIB_DIR
	done | cut -d '(' -f1 | sort -u | cut -d= -f1 | tr -d '\t' | tr -d ' '
}

case $1 in
	qt5)
		QT_PLUGINS="$QT5_PLUGINS"
		;;
	qt6)
		QT_PLUGINS="$QT6_PLUGINS"
		;;
	*)
		echo "Usage: $0 qt5|qt6" >&2
		exit 1
esac

EXE_DLLS=$(ldd $EXE 2>&1 | grep $LIB_DIR | cut -d= -f1 | tr -d '\t' | tr -d ' ')
VLC_DLLS=$(vlc_plugins_dlls)
QT_DLLS=$(qt_plugins_dlls)


mkdir -p "$OUT_DIR/plugins"
for i in $VLC_PLUGINS; do
	mkdir "$OUT_DIR/plugins/$i"
	cp "$VLC_PLUGINS_DIR/$i/"*.dll "$OUT_DIR/plugins/$i"
done

for i in $QT_PLUGINS; do
	DIR=`dirname "$i"`
	mkdir -p "$OUT_DIR/$DIR"
	cp "$QT_PLUGINS_DIR/$i" "$OUT_DIR/$DIR"
done

for i in $EXE_DLLS; do
	cp "$LIB_DIR/$i" "$OUT_DIR"
done
for i in $VLC_DLLS; do
	cp "$LIB_DIR/$i" "$OUT_DIR"
done
for i in $QT_DLLS; do
	cp "$LIB_DIR/$i" "$OUT_DIR"
done
