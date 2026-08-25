#pragma once

#include "ring_buffer.hpp"

#include "miniaudio.h"

// Opens the default mic, dumps samples into the ring, and closes the device
// when this object dies. The callback must not allocate, lock, or print.
class AudioCapture {
public:
    explicit AudioCapture(RingBuffer& ring);
    ~AudioCapture();

    AudioCapture(const AudioCapture&) = delete;
    AudioCapture& operator=(const AudioCapture&) = delete;

    bool ok() const { return ok_; }

private:
    static void on_data(ma_device* device, void* output, const void* input,
                        ma_uint32 frame_count);

    ma_device device_{};
    bool inited_ = false;
    bool ok_ = false;
};
