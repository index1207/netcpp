# AGENTS.md

**netcpp** — a **simple C++20 network library** for Windows, Linux, and macOS.

- **Repo**: <https://github.com/index1207/netcpp> (main branch: `develop`, synced with `origin/develop`)
- **Version**: `0.4` (`project(netcpp VERSION 0.4 ...)` in root `CMakeLists.txt`)
- **Distribution**: published as a [vcpkg](https://github.com/microsoft/vcpkg) port

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
- **Identifier naming**: lowercase `snake_case` for functions, variables, members, enums, and namespaces. Enforced by `.clang-tidy` (`Case: linux`); member prefixes `_` / `m_` are allowed.
- **Linting**: a tuned `.clang-tidy` config exists at the repo root (C++20, cross-platform; lints only `src/` + `include/`). Run manually with `clang-tidy -p build/<preset> src/<file>.cpp`. There is **no `.clang-format`** yet — formatting is done manually and kept uniform.

## Tests

- Framework: **GoogleTest + GoogleMock**; the entry point is `test/main.cpp` (calls `net::native::initialize()` then `RUN_ALL_TESTS`).
- File naming: `test/<component>_tests.cpp` (e.g. `socket_tests.cpp`).
- Tests are placed 1:1 with the source and only build when `NETCPP_TEST=ON`.
- Linux builds enable code coverage (`--coverage` / `-fcoverage-mapping`) and upload it to codecov.

## CI

- `.github/workflows/windows.yml` and `.github/workflows/linux.yml` — build + run tests on push/PR.
- `.github/workflows/cov.yml` — upload coverage to Codecov; runs on `release`, configures bare CMake with `g++-10` + `liburing-dev` (no vcpkg toolchain / preset).
- macOS is configured via presets but has **no CI** job.

## Git

- Commit messages are currently written in casual English (`Fix receive event bug`, `Change accpet async logic`, etc.).
- **Proposed (needs confirmation)**: adopt Conventional Commits (`feat:`, `fix:`, `docs:`, `refactor:`, `test:`).
- Branches: `develop` is the main development branch, synced with `origin/develop`. (`release` is used for coverage builds.)
- `vcpkg` is managed as a git submodule (`.gitmodules`).

## Editor tip

- `compile_commands.json` is generated under `build/*/`; IDE IntelliSense (e.g. CLion) reads it.
- On Windows, CLion's WSL/Remote integration applies the vcpkg toolchain automatically.

---

## Open items / to confirm

- **Formatter**: `.clang-tidy` exists, but there is still **no `.clang-format`**. Decide whether to add one (and wire `clang-format` into CI).
- **Commit convention**: still casual English — confirm whether to formalize Conventional Commits (`feat:`, `fix:`, `docs:`, `refactor:`, `test:`).
- **Minimum compiler docs**: README lists Windows = VS 2019, Linux = Clang 12 / GCC 10; CMake requires 3.23+ and CI uses `g++-10`. These are consistent axes, but keep the README in sync with the toolchains actually tested. (VS 2019's C++20 support is limited — verify it still builds, otherwise bump the stated minimum.)
- **README vs platforms**: README says "It supports windows and linux platform," but `CMakePresets.json` and the non-Windows compile path include **macOS**. Consider updating the README to list macOS.
