#pragma once

#include "note.hpp"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>

// One line, overwritten with '\r' so it feels like a clip-on tuner.
inline void print_tuner_line(const Pitch& p, bool silent) {
    constexpr int kWidth = 21;
    constexpr int kCenter = kWidth / 2;

    std::string bar(kWidth, '-');
    bar[static_cast<std::size_t>(kCenter)] = '|';

    char line[96];
    if (silent || !(p.frequency_hz > 0.0)) {
        std::snprintf(line, sizeof(line), "  --  [%s]              ", bar.c_str());
    } else {
        int pos = kCenter + static_cast<int>(std::lround(p.cents / 50.0 * kCenter));
        if (pos < 0) {
            pos = 0;
        }
        if (pos >= kWidth) {
            pos = kWidth - 1;
        }
        bar[static_cast<std::size_t>(pos)] = '*';

        const char sign = p.cents >= 0.0 ? '+' : '-';
        std::snprintf(line, sizeof(line), "  %s%d  [%s]  %c%.0f cents    ", p.name, p.octave,
                      bar.c_str(), sign, std::abs(p.cents));
    }

    std::cout << '\r' << line << std::flush;
}
