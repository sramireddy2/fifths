# fifths

```
  A2  ───────●────────  +3¢
```

Terminal tuner. You play a note, it tells you what it is and how many cents you're off — same idea as a Snark or Fender Tune, just in the command line.

No mic yet. Right now it fakes a 110 Hz sine (that's A2), pushes it through a lock-free ring buffer, and guesses the pitch with autocorrelation. The note-name / cents math already works.

## stack

| piece | using |
| --- | --- |
| language | C++20 |
| build | CMake 3.20+ |
| audio | [miniaudio](https://miniaud.io) (not dropped in yet) |
| pitch | autocorrelation first, YIN or MPM if it starts picking the wrong octave |
| threads | SPSC ring buffer, `std::atomic`, no mutex on the audio path |

Also the usual modern C++ stuff: `std::vector` with `reserve`, `std::span` for views, RAII around the device once audio exists, `constexpr` for the note table.

## build

```
cmake -S . -B build
cmake --build build
```

Binary lands at `build/fifths`, or `build/Debug/fifths.exe` on Windows.
