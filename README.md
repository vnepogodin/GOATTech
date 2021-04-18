# SportTech-overlay

A cross-platform http static server for SportTech overlay.

## Installation

### Automatic

#### Cargo

You can install the latest version (or a specific commit) of SportTech-overlay directly from GitHub.

```shell
cargo install --git https://github.com/vnepogodin/SportTech-overlay.git
```

[cargo]: https://doc.rust-lang.org/cargo/

### Manual

#### Prebuilt binaries

Archives of prebuilt binaries are available on [GitHub Release][gh-release] for Linux, macOS and Windows. Download a compatible binary for your system.

[gh-release]: https://github.com/vnepogodin/SportTech-overlay/releases

#### Build from source

SportTech-overlay is written in Rust. You need to [install Rust][install-rust] in order to compile it.

```shell
$ git clone https://github.com/vnepogodin/SportTech-overlay.git
$ cd SportTech-overlay
$ cargo build --release
$ ./target/release/sport_tech_overlay
```

[install-rust]: https://www.rust-lang.org/install.html

## Usage

The simplest way to start is to run this command:

```shell
sport_tech_overlay
```

The command above will start serving your current working directory on `0.0.0.0:7000` by default.

## Contributing

Contributions are highly appreciated! Feel free to open issues or send pull requests directly.

## License

This project is licensed under the terms of the GPL-2.0 license, see ([LICENSE](LICENSE) or https://www.gnu.org/licenses/old-licenses/lgpl-2.0.html).
