# DQ Viewer
DQ Viewer is a simple video player especially designated for playing video
streams from Digiteq Automotives FG4 grabber cards and MGB devices.

## Features
* Play local cameras streams (DirectShow / video4linux).
* Play network streams with predefined setup for MGB devices.
* Record the streams to h264 or MPEG2.
* Stream videos to video outputs (Linux only).
* Capture video frames snapshots to PNG.
* Configure FG4 devices.
* Available for Windows & Linux.

## Known Issues
* Due to libVLC missing Wayland integration support, DQ Viewer must be run with
  explicitly setting the xcb Qt platform on Wayland: "`dqview --platform xcb`"

## License
DQ Viewer is licensed under GPL-3.0 (only).
