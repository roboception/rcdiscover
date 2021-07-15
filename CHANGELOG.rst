^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rcdiscover
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.1.4 (2021-07-15)
------------------

* fix version in package.xml

1.1.3 (2021-07-15)
------------------

* use target_compile_features instead of CMAKE_CXX_STANDARD
* fix installation destination for package.xml

1.1.2 (2021-06-14)
------------------

* update cmake files and packaging, requires cmake >= 3.1

1.1.1 (2021-04-16)
------------------

* Ignore network unreachable socket error under Windows

1.1.0 (2021-02-05)
------------------

* Treat all devices as comming from Roboception if model starts with either rc_visard or rc_cube

1.0.5 (2020-10-02)
------------------

* Added building on CI for focal
* persistently store window state

1.0.4 (2020-04-04)
------------------

* only enable reset button and context menu entry for rc_visard devices
* remove catkin build_export_depend from package.xml

1.0.3 (2020-03-22)
------------------

* fix mapping of reachability flag in the GUI
* more generic naming as it can also be used for other GEV devices
* only show rc_visards in "reset" dialog

1.0.2 (2019-12-12)
------------------

* Added filtering by model name in command line tool
* Do not append alternative interface to output of cli if --serialonly or --iponly is given

1.0.1 (2019-09-16)
------------------

* rename CMAKE options from `INSTALL_x` to `BUILD_x`
* improve debian packaging via CPack

1.0.0 (2019-06-04)
------------------

* fix wildcard matching
* allow filtering by interface
* print interface on which the device was found
* also build shared lib for use in other packages
* make it possible to release as ROS third party package

0.9.2 (2019-01-10)
------------------

* GUI: fix WebGUI cannot be opened via context menu (GitHub issue #4)

0.9.1] (2019-01-08)
-------------------

* GUI: fix reading of device name
* CLI: rename 'username' to 'name' and 'serialnumber' to 'serial number' in device list header

0.9.0 (2018-12-19)
------------------

* CLI: print model
* extend CLI interface (SW-302)
  * reset
  * forceip
  * reconnect
* show model on GUI (SW-300)
* GUI: show complete package version in about dialog
* fixed hidden buttons for Ubuntu Bionic

0.8.0 (2018-11-12)
------------------

* add opying device info to clipboard
* add device filter text field
* Changed some dialog workflows
* Added KUKA to manufacturer filter

0.7.0 (2018-01-05)
------------------

* Added possibility to filter for name, serial number and mac address to command line tool
* include <array> in utils

0.6.0 (2017-11-09)
------------------

* add force IP command: setting of temporary IP address
* add `-serialonly` flag for command line tool

0.5.0 (2017-09-29)
------------------

* Optional disabling of RP filtering during installation on Ubuntu
* Directed broadcasts on Windows

0.4.2 (2017-08-31)
------------------

* add Desktop files and icons for debian
* Disabled vectorization and binding to CUDA

0.4.1 (2017-08-21)
------------------

* Fixed bug that prevented the main window from being closed once the help dialog was opened from the reset dialog

0.4.0 (2017-08-04)
------------------

* Help buttons

0.3.2 (2017-07-06)
------------------

* Build console application as Windows console application
* Sign with SHA512

0.3.1 (2017-07-04)
------------------

* Fixed rcdiscover console application

0.3.0 (2017-07-04)
------------------

* Global broadcast instead of directed
* add reachability check via ping

0.2.0 (2017-06-29)
------------------

* Removed IP address form from Reset Dialog

0.1.0 (2017-06-26)
------------------

* Initial release
