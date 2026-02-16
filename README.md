# C++ Client–Server Social Network

This project implements a distributed client–server social networking system written in C++, using a custom TCP-based protocol and persistent storage backed by SQLite.

The system is designed as a multi-user platform supporting authentication, friendship relationships with dynamic status propagation, content sharing with visibility constraints, and both private and group-based messaging.

The focus of the project is on low-level network communication, server-side architecture, modular command handling, and consistent state management across multiple clients.

## Architecture

The application follows a classic client–server model.

The server listens on a TCP socket and manages multiple concurrent client connections. Clients communicate with the server using a text-based protocol, sending commands that are parsed and executed server-side.

Server-side logic is organized around a command-based execution model, following the Command design pattern. Each client request is mapped to a dedicated command handler responsible for executing the corresponding operation and interacting with persistent storage.

Command creation and dispatch are centralized through a factory-based mechanism, enabling extensibility and allowing new operations to be added without modifying the core server loop.

The system is structured according to a three-layer architecture, separating networking concerns, application logic, and data persistence.

## Server Design

The server is responsible for:

- accepting and managing multiple TCP client connections
- parsing incoming client commands
- dispatching requests through a centralized command handling mechanism
- executing application logic
- maintaining authenticated user sessions
- ensuring consistent global state across concurrent clients
- persisting data in an SQLite database

Session management is handled centrally, allowing the server to track authenticated users and enforce access control across all operations.

## Client Implementations

Two client interfaces are provided:

- a command-line client for direct interaction and testing
- a graphical client implemented using Qt Widgets

Both clients communicate with the server using the same text-based protocol and support the full feature set of the system.

## Friendship and State Management

The system implements a relational friendship model with explicit state transitions.

Features include:

- sending and accepting friend requests
- dynamic friendship status updates reflected consistently for all users
- support for multiple friendship types
- global propagation of relationship changes across posts and messaging access

Friendship state directly influences content visibility and messaging permissions, and all relationship changes are propagated consistently across active sessions.

## Posts and Visibility Model

Users can create posts associated with visibility levels:

- public
- friends-only
- close friends

Post visibility is resolved dynamically based on the relationship between users. All posts are persisted and retrieved from the database according to these rules.

## Messaging System

The messaging subsystem supports:

- one-to-one private conversations
- group conversations with multiple participants
- persistent message history
- dynamic group creation
- adding users to existing group conversations

Each conversation maintains its own participant list and message stream, persisted across server restarts.

## Storage Model

All persistent data is stored in an SQLite database.

The database schema includes tables for:

- users
- friendships
- posts
- conversations
- conversation participants
- messages

The server is responsible for managing consistency between in-memory state and persistent storage.

## Layered Structure

The server-side implementation follows a layered architectural approach:

- the networking layer handles TCP communication and connection management
- the application layer implements authentication, friendships, posts, and messaging logic
- the data layer encapsulates database access and persistence logic

This separation improves maintainability, enforces clear responsibilities, and supports future extensibility.

## Build and Execution

The project uses CMake as its build system, enabling a platform-independent build process and clear separation between source and build artifacts.

```bash
cmake -S . -B build
cmake --build build -j$(nproc)

