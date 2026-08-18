# fifths

Terminal tuner. You play a note and it tells you the pitch, the closest note name, and how many cents sharp or flat you are.

Not there yet. Right now this is just the CMake project and the frequency-to-note conversion (`A4 = 440 Hz`). Mic capture and pitch detection come after that.

## build

```
cmake -S . -B build
cmake --build build
```

That should spit out `build/fifths`, or `build/Debug/fifths.exe` if you're on Windows.

CMake 3.20+ and C++20.
