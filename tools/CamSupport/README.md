This directory contains files for camera profile generation.

Prerequisites
-------------
* _DNG converter_. We need to convert the original RAW to DNG.
  Used can be Photoshop or the free DNG converter.
* _ExifTool_. Free tool for extracting data from the RAW files.
* _Python3_. We need to run a Python script to generate HSV maps.
  If Python is not installed, and you don't want to install it,
  then try to use one which is already installed with other
  software (for example LibreOffice).

Files
-----
* _Cam.cpp_ and _Cam.hpp_ are templates for camera profiles.
* _genhsv.py_ is a Python script which generates HSV maps.

Create new camera profile
-------------------------
* First of all: the original camera RAW format must be CR2.
* Convert them into a DNG raw.
* Extract general metadata with ExifTool from the DNG.
```
exiftool name.dng > name.dng.txt
```
* Copy template files and rename them.
* In the template copies, replace `|CAM|` pattern with the
  appropriate camera model ID (for example 5D2). So we get
  a unique profile class name.
* Now fill out the profile data from `name.dng.txt`. Maybe some
  of them can be removed as they are defaults.
* Generate the HSV maps.
```
python3 genhsv.py -c {camera model id} name.dng
```
Note: Some maps can be absent.

Install new camera profile
--------------------------
* Copy the `*.cpp` and `*.hpp` files into the project source dir.
* Add them into VS project and also in the CMake file.
* Register the camera ID in `CamID.hpp`.
* Update the camera profile factories in `CamProfile.cpp`.
* Add support note to the main README.
* Compile and test it on real data...
