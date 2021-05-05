# SportTech-overlay [![CI](https://github.com/vnepogodin/SportTech-overlay/workflows/CI/badge.svg)](https://github.com/vnepogodin/SportTech-overlay/actions/workflows/ci.yml)

Наложение ввода, которое работает без OBS Studio.

## Компиляция

Любой компилятор C++20 должен работать. Для компиляторов с частичной поддержкой C++20 это может сработать. Если ваш компилятор имеет функции C++20, доступные в Visual Studio 2019 / GCC 10.2, он будет работать.

Чтобы построить графический интерфейс, вам нужен Qt.

Существует несколько вариантов компиляции:
* cmake - кросс-платформенный инструмент для сборки
* Windows: Visual Studio (VS 2019 и выше)
* Windows: Qt Creator + msvc
* gnu make
* g++ 10.2 (или более поздняя версия)
* clang++

### cmake

Пример, компиляция SportTech-overlay с cmake:

``shell
cmake -S . -B build
cmake --build build
```

Если вы хотите скомпилировать релизную сборку.
-DCMAKE_BUILD_TYPE=Release

С помощью cmake вы можете создавать файлы проектов для Visual Studio,XCode и т.д.

## Использование

Самый простой способ запуска-выполнить эту команду:

``shell
overlay
```

## Contributing

Contributions are highly appreciated! Feel free to open issues or send pull requests directly.

## License

This project is licensed under the terms of the GPL-2.0 license, see ([LICENSE](LICENSE) or https://www.gnu.org/licenses/old-licenses/lgpl-2.0.html).
