RawDev
======
*Digital camera RAW image developer*

Version 2.0, April 2025

[![Build and test](
https://github.com/petrk23/RawDev/actions/workflows/build.yml/badge.svg)](
https://github.com/petrk23/RawDev/actions/workflows/build.yml)

What is RawDev
--------------
RawDev is a command-line tool that takes RAW file from the camera
and convert its data into a final TIFF file photo. As result, we
have an RGB image that we can work with.

There is a variety of processing options, so that we can match
the output as close as possible to the desired final output.
These options are the same as in Adobe Camera Raw (ACR) which
is the reference for this program (workflow and quality).

To show the available options on Windows run
```
RawDev.exe /h
```
For Unix-like systems
```
./RawDev -h
```

Motivation
----------
Maybe you ask: Why make yet another RAW developer?

Well, that is a good question. They are excellent commercial
tools on the market. If I would try to create a competing
solution, I would spend many years developing a piece of software
that is already there. And maybe I would never reach the quality
of the professional solutions.

The motivation was to build my own solution to learn and explore
how my camera/software makes photos from these RAW files. This
knowledge would help me to understand how things work and how I can
work with that format (in the field or in ACR).

In every step of the development, I asked myself: what benefit
would I have if I spent my time to develop some feature? That is
one reason why I skipped the denoising. Denoising is interesting.
Sure. But it gives nothing in terms of understanding RAW
development.

Despite this selective implementation, the software produces
high-quality images that can be used.

And finally, it would be great to realize some photo projects
by using my own software!

Supported RAW format
--------------------
As I use only Canon cameras, the development was focused on
those. They have Canon RAW format version 2. So the only
supported format is CR2. Maybe I extend this in the future,
but at this moment I don't see any benefit in implementing
support for others.

The software could read files from all cameras that have CR2
format as output. No problem. The problem is that every camera
need specific processing and profile. Every generalization would
result in sacrificing quality of the final image output.
Therefore, the set of supported cameras is limited.

Only full RAW format is supported. No mRAW or sRAW.

Dual-pixel RAW files are also supported, despite the fact that
we don't take full advantage of that technology.

Supported cameras
-----------------
* Canon 1DX line
    * Canon EOS 1DX
    * Canon EOS 1DX Mark II
* Canon 5D line
    * Canon EOS 5D
    * Canon EOS 5D Mark II
    * Canon EOS 5D Mark III
    * Canon EOS 5D Mark IV
    * Canon EOS 5Ds
    * Canon EOS 5Ds R
* Canon 6D line
    * Canon EOS 6D
    * Canon EOS 6D Mark II
* Canon 7D line
    * Canon EOS 7D
    * Canon EOS 7D Mark II
* Other Canon cameras
    * Canon EOS 77D
    * Canon EOS 80D

Build and install
-----------------
See `BUILD.md` file for instructions.

License
-------
License is GPLv3 or later. See `LICENSE` file.

Note about (missing) denoise
----------------------------
In most cases, we do not need any noise reduction. There are some
good CFA-based denoising algorithms, but they are very complicated.
Using primitive denoising would make more problems than benefits
and degrade the HQ outputs. And if we don't use the CFA advantage,
then we could direct denoise in Photoshop or any other tool.

Why not using GPU
-----------------
The most time-consuming parts of the program are AHD demosaicing
and CR2 decoding. Any of these is very hard to implement to run
effectively on a GPU. The CPU versions run in seconds, so only
the copying to/from the GPU memory would ruin our hypothetical
speedup. _Therefore, there is no GPU computation in this project._

Usage of `clang-tidy` and `clang-format`
----------------------------------------
*The usage of these tools is highly recommended!*

Sometimes it's hard, and maybe wrong, to use the recommendation.
For me, the output of these tools is only a hint, that there could
be potential issues in my code. This is also the reason I don't
want to enforce their usage (pipelines, hooks, etc.).

The target here is to minimize warnings of `clang-tidy` and use
as close as possible formatting with `clang-format`. Always think
and evaluate what these programs recommend before adopting it!

FAQ
---
#### Why the name RawDev?

Because it describes what the program does: RAW image development.

#### Why PascalCase is used for the program name?

Why using `RawDev` instead of `rawdev`? Because I want to
minimize confusion with programs working on/with raw devices
(yes, they exist for some Unix-like systems). So I chose the
PascalCase name RawDev, which distinguishes it from other tools
on Unix. See it as a product name, which is consequently used
inside the whole project.

#### Why we begin with version 2.0? What was before?

Before version 2.0, there were two versions of RawDev. The first
one was a private repository where the project development
happened. The second was a limited demo version based on version
1.2, here on GitHub, which was not maintained. After completing
version 1.18, I've decided to have only one, which is public and
maintained here on GitHub. Reaching this milestone, let me
advance the version number to 2.0.

#### How to process multiple images

The program is a command-line program, so we can use shell
scripting to process a multiple image project.

#### Why my images are not rotated correctly

This feature is not implemented. RawDev doesn't read EXIF
information, so it's unknown what the rotation was. We must do it
afterward in any viewer or editor. Pretty much all of them can do
this, even the simplest ones...
