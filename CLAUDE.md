# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Current state

The repository is a scaffold. [src/](src/) is empty and there is no build system, README, or configuration file. Treat any source/build setup as a greenfield decision and confirm with the user before introducing one (CMake vs. Meson vs. plain compiler invocations, C++ standard, etc.).

## Vendored dependencies

Third-party libraries live under [3rdparty/include/](3rdparty/include/) as in-tree headers, not as submodules or a package-manager manifest. Any build configuration must add `3rdparty/include` to the include path.

- **Catch2 v3 (amalgamated)** — [3rdparty/include/catch2/](3rdparty/include/catch2/). Both the header and the `.cpp` are present, so tests link by compiling `catch_amalgamated.cpp` into the test binary (no separate Catch2 library build, no `CATCH_CONFIG_MAIN` macro — v3 provides its own `main`).
- **nlohmann/json (single-header)** — [3rdparty/include/nlohmann/](3rdparty/include/nlohmann/). Include `<nlohmann/json.hpp>`; `json_fwd.hpp` is available for forward declarations in headers.
