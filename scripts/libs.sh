#!/bin/sh

EXE="dqview.exe"
LIB_DIR="/mingw64/bin"
VLC_PLUGINS_DIR="/mingw64/lib/vlc/plugins"
VLC_PLUGINS="access access_output audio_output codec d3d9 d3d11 demux mux packetizer stream_filter stream_out video_filter video_chroma video_output"
QT_PLUGINS_DIR="/mingw64/share/qt5/plugins"
QT_PLUGINS="platforms/qwindows.dll styles/qwindowsvistastyle.dll"
OUT_DIR="."

vlc_plugins_dlls() {
	for i in $VLC_PLUGINS; do
		cd "$VLC_PLUGINS_DIR/$i"
		for j in *.dll; do
			ldd "$j" | grep $LIB_DIR
		done
	done | cut -d '(' -f1 | sort -u | cut -d= -f1 | tr -d '\t' | tr -d ' '
}

qt_plugins_dlls() {
	for i in $QT_PLUGINS; do
		ldd "$QT_PLUGINS_DIR/$i" | grep $LIB_DIR
	done | cut -d '(' -f1 | sort -u | cut -d= -f1 | tr -d '\t' | tr -d ' '
}

EXE_DLLS+=$(ldd $EXE | grep $LIB_DIR | cut -d= -f1 | tr -d '\t' | tr -d ' ')
VLC_DLLS=$(vlc_plugins_dlls)
QT_DLLS=$(qt_plugins_dlls)


mkdir -p "$OUT_DIR/plugins"
for i in $VLC_PLUGINS; do
	cp -r "$VLC_PLUGINS_DIR/$i" "$OUT_DIR/plugins"
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
