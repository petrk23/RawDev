How to build and install RawDev
===============================

Prerequisites
-------------
RawDev itself has no dependencies, but for testing,
GoogleTest is used.

What we need:
* Git for cloning the repository.
* CMake from version 3.20 upwards.
* C++20 and CMake-compatible compiler.
* Internet connection for downloading GoogleTest by CMake.

Build with CMake
----------------
This is the recommended build method for Windows
and Unix-like systems.

1) Git clone the repository.
```
git clone https://github.com/petrk23/RawDev.git
```
2) Go to the project root directory.
```
cd RawDev
```
3) Configure the project with CMake:
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
```
4) Then build it:
```
cmake --build build
```
That was it! The code should be build and the result
is in the `build` directory.

Run automated tests
-------------------
This step is optional for normal users.

Go to `build` directory and run `ctest`. All available tests
will be run. All of them must pass!

Installing
----------
RawDev is a single executable. So the installation is done
by simply copying it to the target destination. On Windows,
I recommend using `Program Files` as destination.

We can also use CMake to do this.
```
cmake --install build
```
The command above installs RawDev to the correct system directory.
This will, of course, require having elevated user privileges.

For local installation, use the `--prefix` switch.
```
cmake --install build --prefix <install dir>
```

Alternative for Windows
-----------------------
In the `vsproj` directory there are Visual Studio project files
and solution, which can be used to build in Visual Studio or by
the `msbuild` tool. Select release target and build...

Problems with 32bit architectures
---------------------------------
We can use it, but with high-megapixel cameras, we run out of memory.
Therefore, available memory limits the usability.

Tips and tricks
---------------
* In the configuration stage, use Ninja build tool (`-G Ninja`).
  This makes the build much faster.
* We can use different compilers by using
  `-DCMAKE_CXX_COMPILER=compiler` in the configuration stage.
* For clean configuration, use the `--fresh` switch or remove
  the `build` directory.
* To clean the solution run `cmake --build build -t clean`.
