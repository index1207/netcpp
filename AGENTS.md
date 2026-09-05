# AGENTS.md

**netcpp** — a **simple C++20 network library** for Windows, Linux, and macOS.

```
netcpp/
├── include/net/*.hpp   # public headers (API)
├── src/*.cpp           # implementation (1:1 with headers)
├── test/*.cpp          # GTest unit tests (1:1 with sources + main.cpp)
├── build/              # CMake build artifacts (git-ignored)
└── vcpkg/              # vcpkg submodule (dependency manager)
```

- Language: **C++20** (`CMAKE_CXX_STANDARD 20`)
- Platforms: Windows (Winsock2 / ws2_32), Linux/macOS (liburing)
- Build: **CMake 3.23+**, vcpkg toolchain, environments managed via `CMakePresets.json`
- Tests: **GoogleTest** (FetchContent v1.15.2) + CTest; Linux builds collect coverage via codecov

## Commands

Pick a build environment (preset): `win-x86_64`, `linux-x86_64`, `mac-arm64` × `-debug` / `-release`

```powershell
# Configure (uses a preset — applies the vcpkg toolchain automatically)
cmake --preset win-x86_64-release

# Build
cmake --build build/win-x86_64-release --config Release

# Run tests (via CTest)
cmake --build build/win-x86_64-release --target test
# or
ctest --test-dir build/win-x86_64-release

# Install (for use in another project)
cmake --install build/win-x86_64-release --prefix <PATH>
```

Key CMake options (`-D`):

| Option | Description |
|--------|-------------|
| `NETCPP_BUILD_SHARED` | Build as a shared library (default: static) |
| `NETCPP_TEST` | Include unit tests in the build (default: `ON`) |

Consume the library from another project:

```cmake
find_package(netcpp CONFIG REQUIRED)
target_link_libraries(main PRIVATE netcpp::netcpp)
```

## Project layout & module conventions

- **1:1 file rule**: `include/net/X.hpp` ↔ `src/X.cpp` ↔ `test/X_tests.cpp` (e.g. `socket` → `socket.cpp` → `socket_tests.cpp`). When adding a new component, add all three files together.
- **Namespace**: all code lives inside `namespace net`.
- **Public API**: keep headers under `include/net/` only, and mark them with `NETCPP_API` (export/import).
- **Platform branching**: split Windows/Linux with `#ifdef _WIN32`. Windows uses OVERLAPPED + IOCP; Linux uses io_uring.
- **Dependencies**: Windows=`ws2_32` (Winsock2), Linux/POSIX=`liburing`.

## C++ style

- Follow **C++20**. Prefer standard containers/views (`std::span`, `std::optional`, `std::function`).
- Use `#pragma once` in headers.
- Include order: standard headers → project headers (`net/...`) → platform headers.
- Mark public class members explicitly with `public:`/`private:` sections.

## Tests

- Framework: **GoogleTest + GoogleMock**; the entry point is `test/main.cpp` (calls `net::native::initialize()` then `RUN_ALL_TESTS`).
- File naming: `test/<component>_tests.cpp` (e.g. `socket_tests.cpp`).
- Tests are placed 1:1 with the source and only build when `NETCPP_TEST=ON`.
- Linux builds enable code coverage (`--coverage` / `-fcoverage-mapping`) and upload it to codecov.

## Git

- Commit messages are currently written in casual English (`Fix receive event bug`, `Change accpet async logic`, etc.).
- **Proposed (needs confirmation)**: adopt Conventional Commits (`feat:`, `fix:`, `docs:`, `refactor:`, `test:`).
- Branches: `develop` is the main development branch, synced with `origin/develop`.
- `vcpkg` is managed as a git submodule.

## Editor tip

- `compile_commands.json` is generated under `build/*/`; IDE IntelliSense (e.g. CLion) reads it.
- On Windows, CLion's WSL/Remote integration applies the vcpkg toolchain automatically.

---

## [TBD / Needs confirmation]

- **Formatter/linter**: no `.clang-format`, ESLint, or similar configuration exists in the project. Decide whether to add one.
- **Commit convention**: confirm whether to formalize Conventional Commits as proposed above.
- **Minimum compiler**: `CMakeLists.txt` requires CMake 3.23, but the README states "VS 2019" as the minimum — the docs and reality do not match. Needs correction.
