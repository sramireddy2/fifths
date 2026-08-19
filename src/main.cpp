#include "config.hpp"
#include "detect.hpp"
#include "note.hpp"
#include "ring_buffer.hpp"

#include <cmath>
#include <iostream>
#include <numbers>
#include <vector>

int main() {
    std::cout << "fifths\n\n";

    RingBuffer ring(kRingCapacity);

    // Stand-in for the mic: one window of a pure A2.
    const double played_hz = 110.0;
    std::vector<float> fake_mic(kWindow);
    for (std::size_t i = 0; i < fake_mic.size(); ++i) {
        fake_mic[i] = static_cast<float>(std::sin(2.0 * std::numbers::pi * played_hz *
                                                 static_cast<double>(i) / kSampleRate));
    }

    ring.write(fake_mic);

    std::vector<float> window(kWindow);
    const std::size_t got = ring.read(window);
    const double heard_hz = estimate_frequency({window.data(), got});

    std::cout << "played  " << format_pitch(frequency_to_pitch(played_hz)) << '\n';
    std::cout << "heard   " << format_pitch(frequency_to_pitch(heard_hz)) << '\n';
    return 0;
}
