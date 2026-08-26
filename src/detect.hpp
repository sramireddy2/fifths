#pragma once

#include "config.hpp"

#include <cmath>
#include <span>
#include <vector>

// How loud the window is. Quiet rooms sit near 0; a played note is bigger.
inline float rms(std::span<const float> samples) {
    if (samples.empty()) {
        return 0.0f;
    }
    double acc = 0.0;
    for (float x : samples) {
        acc += static_cast<double>(x) * static_cast<double>(x);
    }
    return static_cast<float>(std::sqrt(acc / static_cast<double>(samples.size())));
}

// YIN (de Cheveigné & Kawahara): difference function, cumulative-mean
// normalize, then take the first dip below the threshold. Unnormalized
// autocorrelation prefers short lags, so a low E looked like ~1 kHz.
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

    // Same number of terms at every tau so longer periods aren't penalized.
    const std::size_t yin_w = n - max_lag;
    if (yin_w < 2) {
        return 0.0;
    }

    std::vector<double> diff(max_lag + 1, 0.0);
    for (std::size_t tau = 1; tau <= max_lag; ++tau) {
        double sum = 0.0;
        for (std::size_t j = 0; j < yin_w; ++j) {
            const double delta = static_cast<double>(samples[j]) -
                                 static_cast<double>(samples[j + tau]);
            sum += delta * delta;
        }
        diff[tau] = sum;
    }

    std::vector<double> cmnd(max_lag + 1, 1.0);
    double running = 0.0;
    for (std::size_t tau = 1; tau <= max_lag; ++tau) {
        running += diff[tau];
        if (running > 0.0) {
            cmnd[tau] = diff[tau] * static_cast<double>(tau) / running;
        }
    }

    constexpr double kThreshold = 0.15;
    std::size_t tau_est = 0;
    for (std::size_t tau = min_lag; tau <= max_lag; ++tau) {
        if (cmnd[tau] < kThreshold) {
            while (tau + 1 <= max_lag && cmnd[tau + 1] < cmnd[tau]) {
                ++tau;
            }
            tau_est = tau;
            break;
        }
    }
    if (tau_est == 0) {
        tau_est = min_lag;
        for (std::size_t tau = min_lag + 1; tau <= max_lag; ++tau) {
            if (cmnd[tau] < cmnd[tau_est]) {
                tau_est = tau;
            }
        }
    }

    double lag = static_cast<double>(tau_est);
    if (tau_est > 1 && tau_est < max_lag) {
        const double a = cmnd[tau_est - 1];
        const double b = cmnd[tau_est];
        const double c = cmnd[tau_est + 1];
        const double denom = a - 2.0 * b + c;
        if (std::abs(denom) > 1e-12) {
            lag += 0.5 * (a - c) / denom;
        }
    }
    if (!(lag > 0.0)) {
        return 0.0;
    }
    return kSampleRate / lag;
}
