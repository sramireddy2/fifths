# fifths

Terminal tuner. You play a note and it tells you the pitch, the closest note name, and how many cents sharp or flat you are.

No mic yet. It fakes a 110 Hz sine (A2), pushes it through a lock-free ring buffer, then guesses the pitch with autocorrelation.

## build

```
cmake -S . -B build
cmake --build build
```

That should spit out `build/fifths`, or `build/Debug/fifths.exe` if you're on Windows.

CMake 3.20+ and C++20.
