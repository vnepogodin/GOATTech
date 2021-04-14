SportTech-overlay
=================

A cross-platform http static server for SportTech overlay.

How to build?
============

**Boost(>= 1.70) should be installed.**

Windows
-------
Open .sln in vs2015, change include dir to yours, then compile.

Linux
-----
Install boost if needed.
``` shell
cd $BOOST_ROOT
./bootstrap.sh --with-libraries=system
sudo ./b2 threading=multi link=static address-model=64 variant=release --prefix=/usr/local -d0 install
```
```shell
g++ -std=gnu++17 -O3 -DNDEBUG src/*.cpp -Iinclude -pthread -lboost_system -o server
```
