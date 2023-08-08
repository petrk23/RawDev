 RawDev
========
*Digital camera RAW image developing*

Version 1.2GH, August 2023

Author: Petr Krajník

### Important note
This is a&nbsp;*special version of RawDev for presentation
purposes*. Therefore, the suffix GH (GitHub). It contains only 
the main app core. The full project contains more stuff like
unit testing, CUDA GPU-accelerated parts, etc. These parts are, 
in my opinion, not ready to be published, or other circumstances, 
like licenses, complicate the publication.

 What is RawDev
----------------
RawDev is a&nbsp;command-line tool that takes RAW files
from cameras and converts their data into a&nbsp;final TIFF
file photo. As result we have a&nbsp;RGB image with
that we can work further. In Photoshop or Gimp.

There are a&nbsp;variety of processing options so that
we can match the output as closely as possible to the
desired final output. These options are the same
as in Adobe Camera Raw (ACR) which is the reference
for this program (workflow and quality).

To show the available options on Windows run
```
RawDev.exe /h
```
For Unix-like systems
```
./RawDev -h
```

 Motivation
------------
Maybe you ask: Why make yet another raw developer?

Well, that is a&nbsp;good question. They are excellent
commercial tools on the market. If I&nbsp;would try to
create a&nbsp;competing solution, I&nbsp;would spend many years
developing a&nbsp;piece of software that is already there.
And maybe I&nbsp;would never reach the quality of the
professional solutions.

The motivation was to build my own solution
to learn and explore how my camera/software
make my photos from these RAW files. This knowledge
would help me understand how things work and how
I&nbsp;can work with that format (in the field or in ACR).

In every step of the development, I&nbsp;asked myself:
what benefit would I&nbsp;have if I&nbsp;spent my time developing
some feature? That is the reason I&nbsp;skipped the
denoising. Denoising is interesting. Sure. But it gives
me nothing in terms of understanding RAW development.

Despite this selective implementation, the software
produces high-quality images that can be used.

 Supported RAW format
----------------------
As I&nbsp;use only Canon cameras, the development
was focused on those cameras. They have Canon RAW
format version&nbsp;2. So the only supported format is CR2.
Maybe I'll extend this in the future, but at this moment
I&nbsp;don't see any benefit in implementing support for others.

The software could read files from all cameras that have CR2
format as output. No problem. But every camera needs specific
processing, so every generalization would result in
sacrificing the quality of the final image output.

Only the full RAW format is supported. No mRAW or sRAW.

Dual-pixel RAW files are also supported, despite the fact
that we don't take full advantage of that technology.

 Supported cameras
-------------------
* Canon 5D line
    * Canon EOS 5D
    * Canon EOS 5D mark II
    * Canon EOS 5D mark III
    * Canon EOS 5D mark IV
    * Canon EOS 5Ds
    * Canon EOS 5Ds R
* Canon 6D line
    * Canon EOS 6D
    * Canon EOS 6D mark II
* Canon 7D line
    * Canon EOS 7D
    * Canon EOS 7D mark II

 Multiple file processing
--------------------------
If we make panoramic photos, we make them from multiple raw 
files that are merged together into one photograph. Maybe 
we use some sort of bracketing or other advanced techniques, 
as I&nbsp;do. But how to do it with this tool?

I've considered making my own sidecar format, as ACR uses.
It could be based on XML or JSON. But then I&nbsp;imagined, that
in reality we don't need them. We have a&nbsp;more powerful
tool for doing that...

We have the shell scripting engine of our operating system.
And as we use a&nbsp;command-line tool, we could use them. We make 
a&nbsp;building script where all the build instructions are
stored. For example bracketing groups etc.

When some working example is ready I&nbsp;will add it here. TODO.

 Image rotation
----------------
This is one thing that was intentionally omitted. It is handy
to have the images rotated as they should be, but this would 
add complexity to the image data writers. For that reason,
I've decided to leave the operation of rotating the image to 
another software, as this basic operation can do they all...

 Compile and run on Unix
-------------------------
The main development was on Windows. So the Unix support came 
a little late. I've tested the program to run Unix-based 
operating systems. That required to make many changes to 
the codebase, so it is now platform independent. Also the 
compilers on these systems raised some warnings that 
the Microsoft compiler hasn't.

The program was compiled with `G++` and `CLANG++` with 
the options `-Wall -pedantic`. Without any warnings!

The memory correctness was verified with `valgrind`.
The program seems to work properly with memory as we
don't get any warnings or memory leaks.

The testing OS was Fedora Linux 38 Workstation.

### Notes about the Makefile
To build the program, we have a&nbsp;Makefile. We also have 
a tool script named `makemake.sh` which (re)generates 
the makefile capturing the source/header file dependencies.
When we make some changes to the project files we should 
consider to rebuild the makefile to keep the dependencies up 
to date.

The make script gives its output to `stdout`, so it should 
be redirected to the makefile file. The command could look
like this

```
tools/makemake.sh > Makefile
```
 License
---------
License is GPLv3 or later. See `LICENSE.md`.
