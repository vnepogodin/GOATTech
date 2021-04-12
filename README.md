StaticServer
==================

A cross-platform http static server based on boost asio example http server2. Now it's supporting large file(> 4G) downloading.

How to build?
============

**Boost(>= 1.50) should be installed.**

Windows
-------
Open .sln in vs2015, change include dir to yours, then compile.

Linux
-----
Install boost if needed.
``` shell
cd $BOOST_ROOT
./bootstrap.sh --with-libraries=system,filesystem,thread
sudo ./b2 threading=multi link=static --prefix=/usr/local -d0 install
```
```shell
g++ -std=gnu++2a -O3 -DNDEBUG src/*.cpp -Iinclude -pthread -lboost_system -lboost_filesystem -o server
```
