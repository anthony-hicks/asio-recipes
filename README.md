# ASIO Recipes
Collection of recipes that supplement the official documentation.

## Recipes

### Echo
#### Blocking client + server
#### Async server
#### Async client
#### Async server using coroutines
#### Async client using coroutines

### GUI
TODO: What should the GUI do? Should be a real example so I run into all of the real
problems. But it shouldn't take away from the ASIO recipes too much.

#### GUI + Blocking server
#### GUI + Async server
#### GUI + Blocking client
#### GUI + Async client
#### GUI + Async server + Async client
#### GUI User input toggles behavior of async client
User hits a button that stops the async client from writing to the server, and does
something else instead, like possibly reading the data to send from a file.

Note 3/18: I think it makes sense to just change what function is generating the messages.
I think it's ok to disconnect from the GPS if that makes it simpler (combining accept AND read loop)
to cancel.
- if switched to file:
    cancel chain of GPS reads: if has own thread, we can just call cancel() on the io ctx
        but if all net happens on same thread, we'd need to close the socket or acceptor or something
        specific to our chain.
    add work to bg thread to read from file, but should check stop_token b/w each iteration so when
    we change back to GPS we can stop without consuming the whole file

Note: Is cancellation too complex
to even try? If so, we could just post a handler into separate work queues and the GUI
chooses which queue based on the GUI state. But the GUI still shouldn't be executing
the handler, so all it does is tell a background thread which queue to pull from.
The thread submitting the handlers should place the rate inside of the handler, too.

^ This sounds right? Each "queue" could just be a diff io_context.

- Example with coroutines

