# SportTech-overlay [![CI](https://github.com/vnepogodin/SportTech-overlay/workflows/CI/badge.svg)](https://github.com/vnepogodin/SportTech-overlay/actions/workflows/ci.yml)

A cross-platform http static server for SportTech overlay.

## Installation

### Automatic

#### Cargo

You can install the latest version (or a specific commit) of SportTech-overlay directly from GitHub.

```shell
cargo install --branch develop --git https://github.com/vnepogodin/SportTech-overlay.git
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

The command above will start serving overlay on `http://localhost:7000` by default.

### Displaying Overlays on your Stream (Adding to OBS)

In order for overlays to display in your OBS client, you need to do the following:

**OBS**
1. Click the '+' under Sources
2. Add a BrowserSource
3. Point it at `http://localhost:7000`
4. Adjust the width/height to be 16:9
5. Click 'Ok' to Save

After you've added the source (and made sure it's visible and on top of your stream), you should start seeing overlays show up. They show up with a transparent background so you won't see anything visible until you start triggering overlays.

## Contributing

Contributions are highly appreciated! Feel free to open issues or send pull requests directly.

## License

This project is licensed under the terms of the GPL-2.0 license, see ([LICENSE](LICENSE) or https://www.gnu.org/licenses/old-licenses/lgpl-2.0.html).
