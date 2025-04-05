TODO list
=========
Last updated 2025-04-01

CMake install
-------------
With CMake, we can install or pack (CPack) the software.
Both features are not used at this moment. This would enable
to install the software by CMake or generate an installer.

Coding style issues and inconsistencies
---------------------------------------
On some places we can find them. Most of them are caused by
code from the origin of the project. Eliminate them!

Review and redesign of ProcRGB module
-------------------------------------
In my opinion, this is the most problematic module of the whole
project. The problematic part is the tone curve modeling code.
This one will need a complete redesign very soon...

Some ideas apart from the above:
* Contrast in Lab color space on L channel. No color shift!
* Separate control of shadows and highlights with contrast.
* Optional process skip.

More extensive automatic testing
--------------------------------
The software overall could be more automatically tested. With
some classes this would require some redesign.

Test on real image
------------------
As part of automatic testing, the software itself should prove
that it works on one of the sample images.

Take full advantage of DualPixel sensors
----------------------------------------
Some supported cameras, like the EOS 5D Mark IV, have dual-pixel
sensors. In the source RAW format, there are two images: one
normally exposed and one underexposed by 1EV. We could use this
for highlight recovery or develop two files naturally bracketed
by the difference.

Read and interpret EXIF data
----------------------------
Decode EXIF embedded data to Canon Raw files. They could be very
useful. We could, for example, read the automatic white balance
value. At this time it's set to default 5000K.

Try to optimize memory usage
----------------------------
Check whether there is a potential to lower the memory footprint.
