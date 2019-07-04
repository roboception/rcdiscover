^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rcdiscover
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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