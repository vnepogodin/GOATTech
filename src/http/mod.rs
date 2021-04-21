// Copyright (C) 2021 Vladislav Nepogodin
//
// This file is part of SportTech overlay project.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

// silence warnings from the `unused_io_amount` clippy lint
#![allow(clippy::unused_io_amount)]

use std::io::prelude::*;
use std::net::TcpListener;
use std::net::TcpStream;
use std::str;

use crate::http::service_pool::ServicePool;

pub mod service_pool;
pub mod worker;

const HTML_STR: &str = r#"
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8" />
    <title>Hello React</title>
  </head>
  <body>
    <div id="root"></div>

    <!-- Load React. -->
    <!-- Note: when deploying, replace "development.js" with "production.min.js". -->
    <script crossorigin src="https://unpkg.com/react@17/umd/react.production.min.js"></script>
    <script crossorigin src="https://unpkg.com/react-dom@17/umd/react-dom.production.min.js"></script>

    <script src="https://unpkg.com/@babel/standalone/babel.min.js"></script>

<!-- First example
    <script type="text/babel">
'use strict';

class LikeButton extends React.Component {
  constructor(props) {
    super(props);
    this.state = { liked: false };
  }

  render() {
    if (this.state.liked) {
      return '♥️';
    }

    return (
      <button onClick={() => this.setState({ liked: true }) }>
        Like
      </button>
    );
  }
}

let domContainer = document.querySelector('#root');
ReactDOM.render(<LikeButton />, domContainer);
</script>
-->
<style>
body {
  margin: 0;
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Roboto', 'Oxygen', 'Ubuntu', 'Cantarell', 'Fira Sans', 'Droid Sans', 'Helvetica Neue', sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
}

code {
  font-family: source-code-pro, Menlo, Monaco, Consolas, 'Courier New', monospace;
}

.App {
  text-align: center;
}

.App-logo {
  height: 40vmin;
  pointer-events: none;
}

@media (prefers-reduced-motion: no-preference) {
  .App-logo {
    animation: App-logo-spin infinite 20s linear;
  }
}

.App-header {
  background-color: #282c34;
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  font-size: calc(10px + 2vmin);
  color: white;
}

.App-link {
  color: #61dafb;
}

@keyframes App-logo-spin {
  from {
      transform: rotate(0deg);
  }
  to {
      transform: rotate(360deg);
  }
}
</style>

<script type="text/babel">
function App() {
  return (
    <div className="App">
    <header className="App-header">
    <img src="https://upload.wikimedia.org/wikipedia/commons/a/a7/React-icon.svg" className="App-logo" alt="logo" />
    <p>
    Edit <code>src/App.js</code> and save to reload.
    </p>
    <a
      className="App-link"
      href="https://reactjs.org"
      target="_blank"
      rel="noopener noreferrer"
    >
    Learn React
    </a>
    </header>
    </div>
  );
}

ReactDOM.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
  document.querySelector('#root')
);
</script>
"#;

/// Handle incoming connection.
fn handle_connection(mut stream: TcpStream) {
    let mut buffer = [0; 1024];
    stream.read(&mut buffer).unwrap();

    const GET: &[u8; 16] = b"GET / HTTP/1.1\r\n";

    let (status_line, contents): (&'static str, &'static str) = if buffer.starts_with(GET) {
        ("HTTP/1.1 200 OK\r\n\r\n", HTML_STR)
    } else {
        ("HTTP/1.1 404 NOT FOUND\r\n\r\n", "404 Not found")
    };
    let response = format!("{}{}", status_line, contents);

    stream.write_all(response.as_bytes()).unwrap();
    stream.flush().unwrap();
}

#[derive(Debug)]
pub struct Server {
    /// Server listener.
    listener: TcpListener,

    /// Pool of workers.
    pool: ServicePool,
}

impl Server {
    /// Create a new server.
    ///
    /// This creates a server to listen on the specified TCP address and port
    #[inline]
    pub fn new(address: &'static str, port: &'static str, pool_size: usize) -> Self {
        let listener = TcpListener::bind(format!("{}:{}", address, port)).unwrap();
        let pool = ServicePool::new(pool_size);

        Self { listener, pool }
    }

    /// Start listening.
    #[inline]
    pub fn run(&self) {
        for stream in self.listener.incoming() {
            let stream = stream.unwrap();

            self.pool.execute(move || {
                handle_connection(stream);
            });
        }
    }
}
