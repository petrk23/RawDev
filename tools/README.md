This directory contains tools and scripts useful for this project.

Camera Support tools
--------------------
The `genhsv.py` is a Python3 tool for extracting camera
specific HSV tables from the DNG file format with ExifTool.
They are needed for adding support for a new camera.

Camera support source (`Cam.cpp`) and header (`Cam.hpp`)
template are here too.

Run clang-tidy on Windows (`run-clang-tidy.cmd`)
------------------------------------------------
This script helps to run `clang-tidy` with a single simple
command on Windows.
