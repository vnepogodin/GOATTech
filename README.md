# SportTech-overlay [![CI](https://github.com/vnepogodin/SportTech-overlay/workflows/CI/badge.svg)](https://github.com/vnepogodin/SportTech-overlay/actions/workflows/ci.yml)

A input overlay which working without OBS Studio.

## Compiling

Any C++20 compiler should work. For compilers with partial C++20 support it may work. If your compiler has the C++11 features that are available in Visual Studio 2019 / GCC 10.2 then it will work.

To build the GUI, you need Qt.

When building the command line tool, [PCRE](http://www.pcre.org/) is optional. It is used if you build with rules.

There are multiple compilation choices:
* cmake - cross platform build tool
* Windows: Visual Studio (VS 2019 and above)
* Windows: Qt Creator + msvc
* gnu make
* g++ 10.2 (or later)
* clang++

### cmake

Example, compiling SportTech-overlay with cmake:

```shell
cmake -S . -B build
cmake --build build
```

If you want to compile a release build.
-DCMAKE_BUILD_TYPE=Release

Using cmake you can generate project files for Visual Studio,XCode,etc.

## Usage

The simplest way to start is to run this command:

```shell
overlay
```

## Contributing

Contributions are highly appreciated! Feel free to open issues or send pull requests directly.

## License

This project is licensed under the terms of the GPL-2.0 license, see ([LICENSE](LICENSE) or https://www.gnu.org/licenses/old-licenses/lgpl-2.0.html).
