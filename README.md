Discovery of roboception sensors
================================

This package contains tools for the discovery of rc_visard sensors via
GigE Vision.

- `rcdiscover`: console application for discovering rc_visards
- `rcdiscover-gui`: graphical application for discovering rc_visards and
  sending magic packets for resetting of parameters


Compiling on Linux
------------------

For compilation of `rcdiscover` nothing special is required.
For `rcdiscover-gui`, however, static libraries of
[WxWidgets](http://www.wxwidgets.org/) must be provided:

### Building of WxWidgets

```
git clone https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets
git checkout v3.1.0  # or other stable version
./configure --disable-shared
make
sudo make install
```

### Building rcdiscover

It's required to do an out-of-source build:
```
mkdir build
cd build
cmake ..
make
```
Afterwards, the binaries can be found in `build/tools/`.

Compiling on Windows
--------------------

### Using MinGW-w64

Install MinGW-w64 by e.g. downloading `mingw-w64-install.exe` from
[here](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/).
(Choosing win32 for threading during setup works; however, posix may work as well.)
Finally, add the `bin` directory of MinGW to your PATH variable.

#### WxWidgets

Static libraries of WxWidgets are required for the rcdiscover-gui. To build
them, the steps from 
[here](https://wiki.wxwidgets.org/Compiling_wxWidgets_with_MinGW) have been
adapted slightly:

```
git clone https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets
git checkout v3.1.0  # or other stable version
cd build\msw
mingw32-make -f makefile.gcc SHARED=0 BUILD=release -j4
```

#### rcdiscover

```
cd rcdiscover
mkdir build-mingw32
cd build-mingw32
cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DwxWidgets_ROOT_DIR=<path to WxWidgets root folder> ..
mingw32-make
```
