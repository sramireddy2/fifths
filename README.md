<div align="center">

# fifths

```
  A2  [--------|*--------]  +3 cents
```

**Play a note.** It tells you the name and how many cents you're off.

[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)
[![miniaudio](https://img.shields.io/badge/audio-miniaudio-1DB954?style=for-the-badge)](https://miniaud.io)
[![YIN](https://img.shields.io/badge/pitch-YIN-FF6B35?style=for-the-badge)](#stack)
[![lock-free](https://img.shields.io/badge/threads-lock--free-6C63FF?style=for-the-badge)](#stack)

</div>

## stack

| piece | using |
| :---: | :--- |
| 🟦 language | C++20 |
| 🛠️ build | CMake 3.20+ |
| 🎙️ audio | [miniaudio](https://miniaud.io) 0.11.25 |
| 🎵 pitch | YIN |
| ⚡ threads | SPSC ring buffer, `std::atomic`, no mutex on the audio path |

## build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run `build/fifths` (Windows: `build/Debug/fifths.exe`). Play a note; quit with ctrl+c. Windows may ask for mic permission the first time.
