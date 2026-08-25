# fifths

```
  A2  [--------|*--------]  +3 cents
```

Terminal tuner. You play a note, it tells you what it is and how many cents you're off — same idea as a Snark or Fender Tune, just in the command line.

It listens on your default mic. Autocorrelation guesses the pitch, then we map that to a note name. If it starts locking onto the wrong octave on a real guitar, that's a known limit of this first detector.

## stack

| piece | using |
| --- | --- |
| language | C++20 |
| build | CMake 3.20+ |
| audio | [miniaudio](https://miniaud.io) 0.11.25 |
| pitch | autocorrelation (YIN later if the octave bugs show up) |
| threads | SPSC ring buffer, `std::atomic`, no mutex on the audio path |

Also: `std::vector` with the analysis window allocated once, `std::span` for views, RAII around the capture device, `constexpr` for the note table.

## build

```
cmake -S . -B build
cmake --build build
```

Binary lands at `build/fifths`, or `build/Debug/fifths.exe` on Windows.

Windows may ask for microphone permission the first time. Play a note; quit with ctrl+c.
