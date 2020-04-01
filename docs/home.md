## TrLib

Welcome to the trlib wiki.

### Other pages with documentation

* [Programming guide](Programming_guide)
* [A description of MiniLabels](mini_labels)
* [Bindings to high level languages](bindings)
* [A Note on the WGS84 datum](WGS84)

### Notes on release

Version 1.1 of trlib is released and compiled binaries for windows are available from the downloads tab!

Links to download will also be available (soon).

### Build procedures

A makefile for linux (gcc) has been added to the root directory. Alternatively you can use [cmake](http://www.cmake.org) to generate makefiles or project-files specific to your system. This uses the `CMakeLists.txt` files in the root-dir and in the `TR_SRC` dir to build a shared library. For example:
* Make a directory BUILD at the same level as trlib. Then cd to that directory.
* Run cmake ../trlib - this will generate makefiles/project files, which defaults to whatever compiler you have on your system (Visual Studio, gcc etc.)
* You can also specify the target compiler suite by using the -G switch: `cmake -G <generator> ../trlib`

### Mini Labels

TrLib uses the (famous) mini label system for specification of projection/reference system metadata. See the [MiniLabels page](minilabels) for an introduction. We plan to implement translation procedures between MiniLabels and other, more widespread, metadata systems such as ogc-wkt and EPSG-codes. A translation from MiniLabels to ESRI-flavoured wkt is already implemented in the core and exposed in the python bindings.

### Supported systems

We have been able to compile and test the code on the following systems:
* Windows XP (Servicepack 3): MINGW (gcc 4.5.2), Visual Studio 2010
* Windows Server 2008 (64 bit): MINGW (gcc 4.5.2), Visual Studio 2010 (cross compiled from WinXP)
* Ubuntu 10.10 (32 bit, i686): gcc 4.4.5
* Ubuntu 10.10 (64 bit, x86_64): gcc 4.4.5
* SunOS (Sparc): Sun C 5.9
* Has also been compiled for Android using the [Android NDK](http://developer.android.com/tools/sdk/ndk/index.html)

### Tested, but not really supported

* Mac OS X, Snow Leopard: gcc 4.2 (**only in single threaded mode**), gcc 4.6 (multithreaded).

### Definition files

The library needs the definition file //def_lab.txt// in order to be able to run. To be able to perform 3d-transformations a geoid library is also needed. Definition file(s) and geoid library will be distributed on releases.

### Auto tests

A rudimentary test suite has been implemented in Python (available in the python folder). The auto tests can be invoked like this:

    python path_to_repo/python/AutoTest.py <shared_library_object> <Geoid_folder>

For example, on windows:

    python trlib\python\AutoTest.py NewlyCompiledTrLib.dll C:\Geoids
