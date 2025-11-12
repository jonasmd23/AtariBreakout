#include <stdlib.h>
#include <math.h>
#include "sound.h"      
#include "tone.h"    

#define LOWEST_FREQ 20U      
#define BIAS 0x80     
#define FULL_SIZE 0XFF
#define BIG_BIT 255.0f
#define ALL 255
#define HALF 2      
#define MIN 0x00

static uint8_t *waveform_buffer = NULL;
static uint32_t buffer_size = 0;
static uint32_t sample_rate = 0;

// Initialize a tone
int32_t tone_init(uint32_t sample_hz)
{
    if (sample_hz < 2 * LOWEST_FREQ) {
        return -1;
    }
    sample_rate = sample_hz;
    sound_init(sample_hz);

    buffer_size = sample_rate / LOWEST_FREQ;
    waveform_buffer = (uint8_t *)malloc(buffer_size * sizeof(uint8_t));
    if (waveform_buffer == NULL) {
        return -2;
    }

    return 0;
}

// Deinitialize a tone
int32_t tone_deinit(void)
{
    if (waveform_buffer != NULL) {
        free(waveform_buffer);
        waveform_buffer = NULL;
        buffer_size = 0;
    }
    sound_deinit();
    return 0;
}

// Start to play a tone
void tone_start(tone_t tone, uint32_t freq)
{
    if (freq == 0 || freq > sample_rate / 2) {
        return;
    }
    uint32_t period_samples = sample_rate / freq;
    if (period_samples > buffer_size) {
        return;
    }

    // Generate waveform in buffer
    for (uint32_t i = 0; i < period_samples; i++) {
        float value = 0.0f;
        float phase = (2.0f * M_PI * i) / period_samples;

        // Switch case to rotate between sound types
        switch (tone) {
            case SINE_T:
                value = BIAS + (sinf(phase) * BIAS);
                break;

            case SQUARE_T:
                value = (i < period_samples / HALF) ? FULL_SIZE : MIN;
                break;

            case TRIANGLE_T:
                if (i < period_samples / 2) {
                    value = (BIG_BIT / (period_samples / HALF)) * i;
                } else {
                    value = BIG_BIT - (BIG_BIT / (period_samples / HALF)) * (i - period_samples / 2);
                }
                break;

            case SAW_T:
                value = (BIG_BIT * i) / period_samples;
                break;

            default:
                value = BIAS;
                break;
        }

        if (value < 0) value = 0;
        if (value > ALL) value = ALL;
        waveform_buffer[i] = (uint8_t)value;
    }

    sound_cyclic(waveform_buffer, period_samples);
}