# Internet Relay Chat Server

## Project Overview

The `ft_irc` project is an implementation of an IRC (Internet Relay Chat) server using C++ 98.

### What is IRC?

Internet Relay Chat (IRC) is a protocol for real-time internet text messaging (chat). It allows users to communicate via text in real-time either through direct private messages or public group channels. IRC networks consist of multiple servers interconnected to form a network where clients can connect to communicate.

### Key Concepts

- **Channels**: Virtual rooms where users can join to participate in group discussions.
- **Private Messages**: Direct communication between two users.
- **Operators**: Users with special permissions in a channel, such as moderating or changing channel settings.
- **Modes**: Channel settings that can affect its behavior, such as requiring a password or limiting the number of users.

## Features

- **Multi-client Handling**: The server supports multiple concurrent clients without hanging or crashing.
- **Non-blocking I/O**: Implements non-blocking I/O operations to ensure responsiveness.
- **Single Polling Mechanism**: Utilizes a single `poll()` or equivalent mechanism for handling all I/O operations.
- **Channel Management**: Supports various channel commands such as KICK, INVITE, TOPIC, and MODE.
- **User Management**: Allows authentication, nickname and username setting, and private messaging.

## Commands

- **Authentication**: Connect to the server using the password provided.
- **Channel Commands**:
  - `KICK`: Remove a user from a channel.
  - `INVITE`: Invite a user to a channel.
  - `TOPIC`: Set or view the channel topic.
  - `MODE`: Change channel modes:
    - `i`: Set/remove invite-only status.
    - `t`: Restrict topic changes to operators.
    - `k`: Set/remove channel key.
    - `o`: Grant or revoke operator privileges.
    - `l`: Set/remove user limit.

### Executable

- Run the server using `./ircserv <port> <password>`.
  - `port`: Port number for listening to incoming connections.
  - `password`: Password required for clients to connect.
