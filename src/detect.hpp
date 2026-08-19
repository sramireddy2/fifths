#pragma once

#include "config.hpp"

#include <span>

// Autocorrelation: try every lag in range and keep the one where the
// signal lines up with itself the most. Period (seconds) = lag / sample_rate,
// so frequency = sample_rate / lag.
inline double estimate_frequency(std::span<const float> samples) {
    const std::size_t n = samples.size();
    if (n < 2) {
        return 0.0;
    }

    const auto min_lag = static_cast<std::size_t>(kSampleRate / kMaxHz);
    auto max_lag = static_cast<std::size_t>(kSampleRate / kMinHz);
    if (max_lag >= n) {
        max_lag = n - 1;
    }
    if (min_lag >= max_lag) {
        return 0.0;
    }

    double best = -1.0e300;
    std::size_t best_lag = 0;

    for (std::size_t lag = min_lag; lag <= max_lag; ++lag) {
        double sum = 0.0;
        const std::size_t count = n - lag;
        for (std::size_t i = 0; i < count; ++i) {
            sum += static_cast<double>(samples[i]) * static_cast<double>(samples[i + lag]);
        }
        if (sum > best) {
            best = sum;
            best_lag = lag;
        }
    }

    if (best_lag == 0) {
        return 0.0;
    }
    return kSampleRate / static_cast<double>(best_lag);
}
