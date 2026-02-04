# Repository Guidelines

## Project Structure & Module Organization
- `server.cpp`: Single-threaded TCP chat server using `select()` to multiplex clients.
- `client.cpp`: Simple TCP chat client that reads from stdin and displays server messages.
- `server/` and `client/`: Present but currently empty; no additional modules or assets live here.

## Build, Test, and Development Commands
This repo does not include a build system. Use `g++` directly:
- Build server: `g++ -std=c++17 -O2 -Wall -Wextra -o server server.cpp`
- Build client: `g++ -std=c++17 -O2 -Wall -Wextra -o client client.cpp`
- Run server (terminal 1): `./server`
- Run client (terminal 2+): `./client`
The server listens on port `1234` and the client connects to `127.0.0.1`.

## Coding Style & Naming Conventions
- C++17; keep includes explicit and minimal.
- Indentation: 2 or 4 spaces are both present; prefer 4 spaces for new edits to improve readability.
- Naming: `snake_case` for variables (`listen_sock`), `UpperCamel` for simple structs (`Client`).
- Keep I/O error handling consistent (use `perror()` or `std::cout`, but not mixed in one block).

## Testing Guidelines
- No test framework is currently present.
- Manual smoke test: start `./server`, connect two clients, exchange messages, and verify `/quit` broadcasts a leave notice.
- If adding tests later, document the framework and add a `tests/` directory.

## Commit & Pull Request Guidelines
- This directory is not a Git repository, so no commit history or conventions are available.
- If you initialize Git, use concise, imperative commit messages (e.g., “Add nickname handshake”).
- PRs should include a short summary, a test note (commands run or “not run”), and any behavior changes.

## Configuration Notes
- Port and host are hard-coded (`1234`, `127.0.0.1`). If you make them configurable, document defaults and example usage.
