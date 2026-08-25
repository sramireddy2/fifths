#pragma once

#include "config.hpp"

// Exponential moving average: each new frequency is mixed with the last one
// so a single noisy frame can't yank the needle across the bar.
class PitchSmoother {
public:
    double push(double hz) {
        if (!have_) {
            value_ = hz;
            have_ = true;
            return value_;
        }
        value_ = kEmaAlpha * hz + (1.0 - kEmaAlpha) * value_;
        return value_;
    }

    void reset() { have_ = false; }

private:
    double value_ = 0.0;
    bool have_ = false;
};
