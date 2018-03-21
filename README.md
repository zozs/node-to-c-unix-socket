# node-to-c-unix-socket

Some example code to setup a UNIX socket listener in C, and then communicate with that
from Node.js. The scenario is as follows:

1. The server process (written in C) is started and listens on a socket.
2. The client process (written in JavaScript) is started.
3. The client connects to the socket, and sends some data.
4. The server responds with data.

Thus, the client will wait for the data to come back.
The typical use case for this implementation will be to call e.g. a
C program, send some data, and then wait for the response from the C program.

## Usage

## Protocol

A very trivial protocol is used.

```
|     len     |  message  |
|   1 byte    | len bytes |
```

Maximum length of message is 256 bytes, because of the protocol above.

## License

See `LICENSE`
