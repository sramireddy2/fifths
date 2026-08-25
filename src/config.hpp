#pragma once

#include <cstddef>

// 44,100 samples per second is what most mics spit out.
inline constexpr double kSampleRate = 44100.0;

// How many samples we look at when guessing a pitch. ~46 ms at 44.1 kHz.
inline constexpr std::size_t kWindow = 2048;

// Ring buffer slots. One stays empty so full and empty are easy to tell apart.
inline constexpr std::size_t kRingCapacity = 8192;

// Search range for autocorrelation. Low E on guitar is ~82 Hz.
inline constexpr double kMinHz = 70.0;
inline constexpr double kMaxHz = 1000.0;

// Below this RMS we treat the window as silence, not a note.
inline constexpr float kSilenceRms = 0.01f;

// Blend of new estimate vs old. 1 = trust this frame only, 0 = never move.
inline constexpr double kEmaAlpha = 0.35;
