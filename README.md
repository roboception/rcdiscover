Discovery of roboception sensors
================================

This package contains tools for the discovery of Roboception devices
(e.g. rc_visard and rc_cube) via GigE Vision.

- `rcdiscover`: console application for discovering Roboception devices
- `rcdiscover-gui`: graphical application for discovering Roboception devices and
  sending magic packets for resetting of parameters

**Installation:** You can find some prebuilt packages for Linux or Windows on the [releases](https://github.com/roboception/rcdiscover/releases) page. Alternatively follow the compilation steps in the next sections.

Compiling on Linux
------------------

For compilation of `rcdiscover` cmake is required.

`rcdiscover-gui` additionally requires [WxWidgets](http://www.wxwidgets.org/).

To install this under Debian/Ubuntu >= 20.04.0:
```
sudo apt-get install cmake libwxgtk3.0-gtk3-dev
```

In older distributions, the package is called libwxgtk3.0-dev

### Building rcdiscover

It's required to do an out-of-source build:

```
mkdir build
cd build
cmake ..
make
```

To build the gui as well, pass the CMAKE option `BUILD_RCDISCOVER_GUI`:

```
cmake -DBUILD_RCDISCOVER_GUI=ON ..
```


Afterwards, the binaries can be found in `build/tools/`.

### Installation

Installation can either be done via

```
make install
```

On Debian (and derivatives like Ubuntu) Debian packages can be built with

```
cmake -DCMAKE_INSTALL_PREFIX="/usr" ..
make package
```
which can be installed with e.g. `sudo dpkg -i rcdiscover*.deb`


Discovering sensors in other subnets
------------------------------------

Most Linux distributions have reverse path filtering turned on, which restricts discoverability of sensor to the same subnet as the host.

Check this with
```
sysctl net.ipv4.conf.all.rp_filter
sysctl net.ipv4.conf.default.rp_filter
```

Reverse path filtering can be turned off with
```
sudo sysctl -w net.ipv4.conf.all.rp_filter=0
sudo sysctl -w net.ipv4.conf.default.rp_filter=0
```
You might also need to disable it for your specific interface, e.g.:
```
sudo sysctl -w net.ipv4.conf.eth0.rp_filter=0
```
Note: These settings are not persistent across reboots!
To persist them you can add a file in `/etc/sysctl.d/` on most distributions.
See `debian/50-rcdiscover-rpfilter.conf` for an example.

If you built a Debian package with `make package`, it will automatically ask you if you want to disable reverse path filtering at package installation.

Compiling on Windows
--------------------

### Using MinGW-w64

Install MinGW-w64 by e.g. downloading `mingw-w64-install.exe` from
[here](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/).
During setup, choose i686 if you want to build 32 bit binaries, or x84_64 for
64 bit. For Threads, select win32. The rest can stay default.

Finally, add the `bin` directory of MinGW to your PATH variable. For 32 bit
installation, it is normally found in
`C:\Program Files (x86)\mingw-w64\i686-7.1.0-win32-dwarf-rt_v5-rev0\mingw64\bin`,
for 64 bit it is
`C:\Program Files\mingw-w64\x86_64-7.1.0-win32-seh-rt_v5-rev0\mingw64\bin`.

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
mingw32-make -f makefile.gcc SHARED=0 BUILD=release -j4 CXXFLAGS="-mtune=generic -mno-abm" CFLAGS="-mtune=generic -mno-abm"
```

#### rcdiscover

```
cd rcdiscover
mkdir build-mingw32
cd build-mingw32
cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_RCDISCOVER_GUI=ON -DwxWidgets_ROOT_DIR=<path to WxWidgets root folder> ..
mingw32-make
```

**For the 32 bit build you may encounter a 0xc000007b error when running
rcdiscover-gui.exe.** This seems to be caused by a bug in WxWidgets build. As
a workaround, rename `rcdefs.h` in `lib\gcc_lib\mswu\wx\msw` in your WxWidgets
root directory to something different (e.g., `rcdefs.h_old`). Then, rerun
above WxWidgets build command:

```
cd build\msw
mingw32-make -f makefile.gcc SHARED=0 BUILD=release -j4 CXXFLAGS="-mtune=generic -mno-abm" CFLAGS="-mtune=generic -mno-abm"
```

Finally, rebuild rcdiscover.
