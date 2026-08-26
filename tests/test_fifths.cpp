#include "config.hpp"
#include "detect.hpp"
#include "note.hpp"
#include "ring_buffer.hpp"
#include "smooth.hpp"

#include <cmath>
#include <iostream>
#include <numbers>
#include <string>
#include <vector>

namespace {

int g_failed = 0;
int g_passed = 0;

void check(bool ok, const std::string& name) {
    if (ok) {
        ++g_passed;
        return;
    }
    ++g_failed;
    std::cerr << "FAIL: " << name << '\n';
}

void check_near(double got, double want, double tol, const std::string& name) {
    const bool ok = std::isfinite(got) && std::abs(got - want) <= tol;
    if (!ok) {
        std::cerr << "  got " << got << ", want " << want << " ± " << tol << '\n';
    }
    check(ok, name);
}

std::vector<float> sine_wave(double hz, std::size_t n, double phase = 0.0) {
    std::vector<float> samples(n);
    for (std::size_t i = 0; i < n; ++i) {
        samples[i] = static_cast<float>(
            std::sin(phase + 2.0 * std::numbers::pi * hz * static_cast<double>(i) / kSampleRate));
    }
    return samples;
}

std::vector<float> plucked_string(double hz, std::size_t n) {
    std::vector<float> samples(n);
    for (std::size_t i = 0; i < n; ++i) {
        const double t = static_cast<double>(i) / kSampleRate;
        samples[i] = static_cast<float>(
            std::sin(2.0 * std::numbers::pi * hz * t) +
            0.55 * std::sin(2.0 * std::numbers::pi * 2.0 * hz * t) +
            0.28 * std::sin(2.0 * std::numbers::pi * 3.0 * hz * t) +
            0.12 * std::sin(2.0 * std::numbers::pi * 4.0 * hz * t));
    }
    return samples;
}

double cents_between(double heard_hz, double expected_hz) {
    return 1200.0 * std::log2(heard_hz / expected_hz);
}

void expect_pitch(double hz, const char* name, int octave) {
    const Pitch p = frequency_to_pitch(hz);
    check(std::string(p.name) == name, std::string("name ") + name + std::to_string(octave));
    check(p.octave == octave, std::string("octave ") + name + std::to_string(octave));
    check_near(p.cents, 0.0, 0.05, std::string("in-tune ") + name + std::to_string(octave));
}

void expect_detected(const std::vector<float>& samples, double expected_hz, const std::string& name) {
    const double heard = estimate_frequency(samples);
    check(heard > 0.0, name + " produced a frequency");
    check_near(cents_between(heard, expected_hz), 0.0, 5.0, name + " within 5 cents");
    const Pitch p = frequency_to_pitch(heard);
    const Pitch want = frequency_to_pitch(expected_hz);
    check(std::string(p.name) == want.name && p.octave == want.octave, name + " nearest note");
}

void test_notes() {
    expect_pitch(440.0, "A", 4);
    expect_pitch(110.0, "A", 2);
    expect_pitch(82.4068892282175, "E", 2);
    expect_pitch(146.8323839587038, "D", 3);
    expect_pitch(196.0, "G", 3);
    expect_pitch(246.94165062806206, "B", 3);
    expect_pitch(329.6275569128699, "E", 4);
    expect_pitch(261.6255653005986, "C", 4);
    expect_pitch(277.1826309768721, "C#", 4);
    expect_pitch(880.0, "A", 5);

    const Pitch silent = frequency_to_pitch(0.0);
    check(!(silent.frequency_hz > 0.0), "zero Hz is not a pitch");
    check(format_pitch(silent) == "—", "silent format");

    const Pitch sharp = frequency_to_pitch(440.0 * std::pow(2.0, 0.10 / 12.0));
    check(std::string(sharp.name) == "A" && sharp.octave == 4, "A4 +10 cents stays A4");
    check_near(sharp.cents, 10.0, 0.05, "A4 +10 cents");

    const Pitch flat = frequency_to_pitch(440.0 * std::pow(2.0, -0.15 / 12.0));
    check(std::string(flat.name) == "A" && flat.octave == 4, "A4 -15 cents stays A4");
    check_near(flat.cents, -15.0, 0.05, "A4 -15 cents");

    const std::string a4 = format_pitch(frequency_to_pitch(440.0));
    check(a4.find("A4") != std::string::npos, "format includes A4");
    check(a4.find("440.00 Hz") != std::string::npos, "format includes Hz");
}

void test_rms() {
    std::vector<float> zeros(kWindow, 0.0f);
    check_near(rms(zeros), 0.0, 1e-12, "rms of silence");
    check(rms({}) == 0.0f, "rms of empty span");

    const auto a4 = sine_wave(440.0, kWindow);
    const float level = rms(a4);
    check(level > 0.5f && level < 0.8f, "rms of unit sine is ~0.707");
    check(level > kSilenceRms, "played note is above silence gate");
}

void test_detector() {
    check_near(estimate_frequency({}), 0.0, 0.0, "empty buffer");
    const float tiny[] = {0.1f};
    check_near(estimate_frequency(tiny), 0.0, 0.0, "too-short buffer");

    constexpr double kE2 = 82.4068892282175;
    constexpr double kA2 = 110.0;
    constexpr double kD3 = 146.8323839587038;
    constexpr double kG3 = 196.0;
    constexpr double kB3 = 246.94165062806206;
    constexpr double kE4 = 329.6275569128699;
    constexpr double kA4 = 440.0;

    const double open_strings[] = {kE2, kA2, kD3, kG3, kB3, kE4, kA4};
    for (double hz : open_strings) {
        expect_detected(sine_wave(hz, kWindow), hz, "sine " + std::to_string(hz));
        expect_detected(sine_wave(hz, kWindow, 1.3), hz, "phase-shifted sine " + std::to_string(hz));
        expect_detected(plucked_string(hz, kWindow), hz, "harmonic " + std::to_string(hz));
    }

    // Original stand-in: one window of a pure A2 through the ring buffer.
    {
        RingBuffer ring(kRingCapacity);
        const auto fake_mic = sine_wave(kA2, kWindow);
        check(ring.write(fake_mic) == kWindow, "ring accepted a full window");
        std::vector<float> window(kWindow);
        const std::size_t got = ring.read(window);
        check(got == kWindow, "ring read a full window");
        expect_detected(window, kA2, "played vs heard A2");
    }

    for (double cents : {-40.0, -12.0, 8.0, 35.0}) {
        const double hz = kA2 * std::pow(2.0, cents / 1200.0);
        const double heard = estimate_frequency(sine_wave(hz, kWindow));
        const Pitch p = frequency_to_pitch(heard);
        check(std::string(p.name) == "A" && p.octave == 2, "offset A2 stays A2");
        check_near(p.cents, cents, 1.0, "A2 offset " + std::to_string(cents) + " cents");
        if (cents > 0.0) {
            check(p.cents > 0.0, "sharp A2 reports +cents");
        } else {
            check(p.cents < 0.0, "flat A2 reports -cents");
        }
    }
}

void test_ring_buffer() {
    RingBuffer rb(8);
    check(rb.write(std::vector<float>{1, 2, 3, 4, 5}) == 5, "write 5");

    std::vector<float> out(3);
    check(rb.read(out) == 3 && out[0] == 1.0f && out[2] == 3.0f, "read 3");
    check(rb.write(std::vector<float>{6, 7, 8, 9, 10}) == 5, "write that wraps");

    std::vector<float> rest(8);
    const std::size_t n = rb.read(rest);
    check(n == 7 && rest[0] == 4.0f && rest[6] == 10.0f, "read wrap-around");

    RingBuffer full(4);
    check(full.write(std::vector<float>{1, 2, 3, 4}) == 3, "one slot stays empty");
    check(!full.push(99.0f), "push when full");

    RingBuffer empty(4);
    float y = -1.0f;
    check(!empty.pop(y), "pop when empty");
}

void test_smoother() {
    PitchSmoother smoother;
    check_near(smoother.push(100.0), 100.0, 1e-12, "first estimate passes through");

    const double blended = smoother.push(200.0);
    const double expect = kEmaAlpha * 200.0 + (1.0 - kEmaAlpha) * 100.0;
    check_near(blended, expect, 1e-12, "ema blend");

    smoother.reset();
    check_near(smoother.push(50.0), 50.0, 1e-12, "reset starts over");
}

}  // namespace

int main() {
    test_notes();
    test_rms();
    test_detector();
    test_ring_buffer();
    test_smoother();

    std::cout << g_passed << " passed, " << g_failed << " failed\n";
    return g_failed == 0 ? 0 : 1;
}
