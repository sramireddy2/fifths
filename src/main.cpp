#include "audio.hpp"
#include "config.hpp"
#include "detect.hpp"
#include "display.hpp"
#include "note.hpp"
#include "ring_buffer.hpp"
#include "smooth.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>
#include <vector>

static std::atomic<bool> g_running{true};

extern "C" void handle_sigint(int) {
    g_running = false;
}

int main() {
    std::signal(SIGINT, handle_sigint);

    RingBuffer ring(kRingCapacity);
    AudioCapture mic(ring);
    if (!mic.ok()) {
        std::cerr << "couldn't open the mic\n";
        return 1;
    }

    std::vector<float> window(kWindow);
    std::size_t filled = 0;
    PitchSmoother smoother;

    std::cout << "play a note (ctrl+c to quit)\n";

    while (g_running) {
        filled += ring.read({window.data() + filled, window.size() - filled});
        if (filled < window.size()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }
        filled = 0;

        const bool quiet = rms(window) < kSilenceRms;
        if (quiet) {
            smoother.reset();
            print_tuner_line({}, true);
            continue;
        }

        const double hz = smoother.push(estimate_frequency(window));
        print_tuner_line(frequency_to_pitch(hz), false);
    }

    std::cout << '\n';
    return 0;
}
