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

`rcdiscover-gui` additionally requires [FLTK](http://www.fltk.org/).

To install this under Debian/Ubuntu >= 20.04.0:

```
sudo apt-get install cmake libfltk1.3-dev
```

### Building rcdiscover

Building steps:

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

Note: These settings are not persistent across reboots! To persist them you can add a file in `/etc/sysctl.d/` on most distributions. See `debian/50-rcdiscover-rpfilter.conf` for an example.

If you built a Debian package with `make package`, it will automatically ask you if you want to disable reverse path filtering at package installation.

Compiling on Windows
--------------------

The main directory of rcdiscover contains the script `build_win.bat`. This script can be run in a Command Prompt for Visual Studio. It was tested with Visual Studio 2022. In the beginning, the script will check for the `git` and `cmake` commands. If these commands are not available, the script provides an URL for downloading them and stops. If the tools are found, the script clones the git repository of FLTK, compiles it and thereafter compiles rcdiscover and rcdiscover-gui. After successful compilation, the executables can be found in the build directory.
