#include "esp_adc/adc_oneshot.h"
#include "joy.h"
#include <stdint.h>

#define NUM_CENTER_SAMPLES 16 

// Global variables
static adc_oneshot_unit_handle_t adc_handle = NULL;
static int_fast32_t joy_center_x = 0;
static int_fast32_t joy_center_y = 0;

// initialize joystick
int32_t joy_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    esp_err_t err = adc_oneshot_new_unit(&init_config, &adc_handle);
    if (err != ESP_OK) {
        return -1;  
    }
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &config);
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_7, &config);

    int val = 0;  // NOLINT
    int_fast32_t x_sum = 0;
    int_fast32_t y_sum = 0;

    // Calculate joystick center by averaging multiple samples
    for (uint_fast8_t i = 0; i < NUM_CENTER_SAMPLES; i++) 
    {
        adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &val);
        x_sum += val;
        adc_oneshot_read(adc_handle, ADC_CHANNEL_7, &val);
        y_sum += val;
    }

    joy_center_x = x_sum / NUM_CENTER_SAMPLES;
    joy_center_y = y_sum / NUM_CENTER_SAMPLES;

    return 0;
}

// Deinitialize joystick
int32_t joy_deinit(void) {
    if (adc_handle != NULL) {
        adc_oneshot_del_unit(adc_handle);
        adc_handle = NULL;
    }
    return 0;
}

// Get joystick displacement relative to center
void joy_get_displacement(int32_t *dx, int32_t *dy)
{
    int val_x = 0;  // NOLINT
    int val_y = 0;  // NOLINT

    adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &val_x);
    adc_oneshot_read(adc_handle, ADC_CHANNEL_7, &val_y);

    if (dx) *dx = val_x - joy_center_x;
    if (dy) *dy = val_y - joy_center_y;
}
