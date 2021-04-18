mod http;

fn main() {
    // Initialize the server.
    const NUM_THREADS: usize = 3;
    let s = http::Server::new("0.0.0.0", "7000", NUM_THREADS);

    // Run the server until stopped.
    s.run();
}
