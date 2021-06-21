# GOATTech

[![CI](https://github.com/vnepogodin/GOATTech/workflows/CI/badge.svg)](https://github.com/vnepogodin/GOATTech/actions/workflows/ci.yml)
[![license](https://img.shields.io/github/license/vnepogodin/GOATTech.svg)](https://github.com/vnepogodin/GOATTech/blob/develop/LICENSE)

A input overlay which working without OBS Studio.

## Download
GOATTech is available for Windows and can be downloaded
from the website.

**[https://torrenttor.ru/GOATTech/download](https://gitreleases.dev/gh/vnepogodin/GOATTech/latest/SportTech-installer.exe)**

## Compiling

Any C++20 compiler should work. For compilers with partial C++20 support it may work. If your compiler has the C++20 features that are available in Visual Studio 2019 / GCC 10.2 then it will work.

To build the GUI, you need Qt. And `CMAKE_PREFIX_PATH` must be specified with qt installation path for Windows build.

### cmake

Example, compiling GOATTech for Windows release build:

```shell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="c:\qt\5.15.2"
cmake --build build --config Release
```

## Usage

Overlay can be hidden by clicking tray once.
Settings can be opened in tray by clicking first line. And second line to exit application.

## Contributing

Contributions are highly appreciated! Feel free to open issues or send pull requests directly.

## License

This project is licensed under the terms of the GPL-2.0 license, see ([LICENSE](LICENSE) or https://www.gnu.org/licenses/old-licenses/lgpl-2.0.html).
