# Fifths

A command-line instrument tuner. Play a note; it reports the pitch, nearest note, and cents sharp or flat.

This first commit is just the skeleton: a CMake build and frequency → note conversion. Live audio and pitch detection come next.

## Build

```bash
cmake -S . -B build
cmake --build build
```

Then run `build/fifths` (or `build/Debug/fifths.exe` on Windows).

Requires CMake 3.20+ and a C++20 compiler.
