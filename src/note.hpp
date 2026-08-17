#pragma once

#include <array>
#include <cmath>
#include <cstdio>
#include <string>

// A4 = 440 Hz is MIDI note 69. n = 12 * log2(f / 440) is semitones from A4.
inline constexpr double kA4Hz = 440.0;
inline constexpr int kA4Midi = 69;

inline constexpr std::array<const char*, 12> kNoteNames{
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

struct Pitch {
    const char* name = "?";
    int octave = 0;
    double cents = 0.0;  // leftover after rounding to the nearest semitone
    double frequency_hz = 0.0;
};

inline Pitch frequency_to_pitch(double hz) {
    Pitch p;
    p.frequency_hz = hz;
    if (!(hz > 0.0)) {
        return p;
    }

    const double semitones_from_a4 = 12.0 * std::log2(hz / kA4Hz);
    const int nearest = static_cast<int>(std::lround(semitones_from_a4));
    p.cents = (semitones_from_a4 - nearest) * 100.0;

    const int midi = kA4Midi + nearest;
    const int wrapped = ((midi % 12) + 12) % 12;
    p.name = kNoteNames[static_cast<std::size_t>(wrapped)];
    p.octave = (midi / 12) - 1;
    return p;
}

inline std::string format_pitch(const Pitch& p) {
    if (!(p.frequency_hz > 0.0)) {
        return "—";
    }
    const char sign = p.cents >= 0.0 ? '+' : '-';
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%s%d  %c%.1f cents  (%.2f Hz)", p.name, p.octave,
                  sign, std::abs(p.cents), p.frequency_hz);
    return buf;
}
