Sprinkler
=========

A software to calculate drop movement using ballistic models.

Authors
-------

* Javier Burguete Tolosa (jburguete@eead.csic.es)

Tools and libraries required to build the executables
-----------------------------------------------------

* [gcc](https://gcc.gnu.org) or [clang](http://clang.llvm.org) (to compile the
source code)
* [make](http://www.gnu.org/software/make) (to build the executable file)
* [autoconf](http://www.gnu.org/software/autoconf) (to generate the Makefile in
different operative systems)
* [automake](http://www.gnu.org/software/automake) (to check the operative
system)
* [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config) (to find the
libraries to compile)
* [gsl](http://www.gnu.org/software/gsl) (to generate random numbers)
* [libxml](http://xmlsoft.org) (to deal with XML files)
* [glib](https://developer.gnome.org/glib) (extended utilities of C to work with
data, lists, mapped files, regular expressions, using multicores in shared
memory machines, ...)

Building instructions
---------------------

This software has been built and tested in the following operative systems.
Probably, it can be built in other systems, distributions, or versions but it
has not been tested.

Debian 8 (Linux, kFreeBSD or Hurd)
__________________________________
DragonFly BSD 4.6
_________________
Dyson Illumos
_____________
Fedora Linux 23
_______________
FreeBSD 10.2
____________
Linux Mint DE 2
_______________
NetBSD 7.0
__________
OpenSUSE Linux 13
_________________
Ubuntu Linux 12, 14, and 15
___________________________

1. Download this repository:
> $ git clone https://github.com/jburguete/sprinkler.git

2. Build doing on a terminal:
> $ ./build

OpenBSD 5.8
___________

1. Select adequate versions:
> $ export AUTOCONF_VERSION=2.69 AUTOMAKE_VERSION=1.15

2. Then, in a terminal, follow steps 1 to 2 of the previous Debian 8 section.

Microsoft Windows 7 (with MSYS2)
________________________________
Microsoft Windows 8.1 (with MSYS2)
__________________________________

1. Install [MSYS2](http://sourceforge.net/projects/msys2) and the required
libraries and utilities. You can follow detailed instructions in
[install-unix]
(https://github.com/jburguete/install-unix/blob/master/tutorial.pdf)

2. Then, in a MSYS2 terminal, follow steps 1 to 2 of the previous Debian 8
section.

