# TCP Chat Application

A simple multi-client chat application implemented in C++ using Windows Sockets (Winsock2).

## Features

- Multi-client support
- Real-time message broadcasting
- User names for chat identification
- Windows platform support
- Console-based interface

## Prerequisites

- Windows operating system
- C++ compiler (e.g., MinGW-w64, Visual Studio)
- WinSock2 library (ws2_32.lib)

## Project Structure

```
TCP_Chat_App/
├── Server/
│   └── Server.cpp
├── Client/
│   └── Client.cpp
└── README.md
```

## Building the Application

### Using MinGW-w64

1. Compile the server:
```bash
cd Server
g++ Server.cpp -o Server.exe -lws2_32
```

2. Compile the client:
```bash
cd Client
g++ Client.cpp -o Client.exe -lws2_32
```

## Running the Application

1. Start the server first:
```bash
cd Server
./Server.exe
```

2. Start one or more clients:
```bash
cd Client
./Client.exe
```

## Usage

1. Run the server first - it will listen on port 12345
2. Run one or more client instances
3. For each client:
   - Enter your chat name when prompted
   - Type messages and press Enter to send
   - Type 'quit' to exit the application
4. Messages from each client will be broadcasted to all other connected clients

## Features

- Multiple simultaneous client connections
- Username support for chat identification
- Real-time message broadcasting
- Simple console-based interface
- Graceful client disconnection handling

## Technical Details

- Uses TCP/IP protocol for reliable communication
- Server listens on 0.0.0.0 (all available network interfaces)
- Default port: 12345
- Supports IPv4 addressing
- Multi-threaded implementation for concurrent client handling

## Contributing

Feel free to fork this project and submit pull requests for any improvements.
