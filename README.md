# ASIO Recipes
Collection of well-documented recipes that supplement the official documentation using modern features
and best practices of C++. No exceptions when possible.

- sync_client.cpp
- blocking_client_mt.cpp
- async_client_mt.cpp
- async_client_mt_coro.cpp

## Blocking IO
TODO: Rewrite all async callback recipes using coroutines
TODO: Just use a table with links to directories here?

## Async IO (callbacks)
## Async IO (coroutines)

### Client: blocking IO, single thread
The simplest you can get.

### Client: blocking IO, multiple threads
TODO: Is this ever a better choice than async IO?

### Client: async IO, single thread
TODO: Not sure what a good use-case for this is on the client side.

### Client: async IO, multiple threads

### Server: blocking IO, single thread
Limited to serving one client at a time.

### Server: blocking IO, multiple threads
Poor man's async?

### Server: async IO, single thread
The poster child of async. A single-threaded server that can scale to handle many, many
clients.

### (1) Server: async IO, multiple threads
The server has another thread that cannot be blocked by IO. A server with a GUI, for example.

### (2) Server: async IO, multiple threads
The server is the main thread but it requires additional threads to do its request processing.
Maybe that's because that processing is expensive. 

