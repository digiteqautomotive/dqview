#!/bin/sh

LIB_DIR="/mingw64/bin"
PLUGINS_DIR="/mingw64/lib/vlc/plugins"
PLUGINS="access access_output audio_output codec d3d9 d3d11 demux mux packetizer stream_filter stream_out video_filter video_chroma video_output"

DLLS=$(
for i in $PLUGINS; do
	cd "$PLUGINS_DIR/$i"
	for j in *.dll; do
		ldd "$j" | grep $LIB_DIR
	done
done | cut -d '(' -f1 | sort -u | cut -d= -f1 | tr -d '\t' | tr -d ' '
)

mkdir "plugins"
for i in $PLUGINS; do
	cp -r $PLUGINS_DIR/$i "plugins"
done

for i in $DLLS; do
	cp $LIB_DIR/$i .
done
