#define MINIAUDIO_IMPLEMENTATION
#include "audio.hpp"

#include "config.hpp"

void AudioCapture::on_data(ma_device* device, void* /*output*/, const void* input,
                           ma_uint32 frame_count) {
    if (input == nullptr || frame_count == 0) {
        return;
    }
    auto* ring = static_cast<RingBuffer*>(device->pUserData);
    ring->write({static_cast<const float*>(input), frame_count});
}

AudioCapture::AudioCapture(RingBuffer& ring) {
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate = static_cast<ma_uint32>(kSampleRate);
    config.dataCallback = on_data;
    config.pUserData = &ring;

    if (ma_device_init(nullptr, &config, &device_) != MA_SUCCESS) {
        return;
    }
    inited_ = true;

    if (ma_device_start(&device_) != MA_SUCCESS) {
        return;
    }
    ok_ = true;
}

AudioCapture::~AudioCapture() {
    if (inited_) {
        ma_device_uninit(&device_);
    }
}
