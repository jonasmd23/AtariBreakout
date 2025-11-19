#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"

// Platform structure
typedef struct {
    float x;                    // Current x position
    float y;                    // Current y position
    float width;                // Platform width
    float height;               // Platform height
    color_t color;              // Platform color
    float move_speed;           // Movement speed
    uint32_t currentState;      // Current state
} platform_t;

/************************ Function Prototypes *************************/

// Initialize platform structure
void platform_init(platform_t *p, uint32_t move_speed);

// Control functions
void platform_get_pos(platform_t *p, float *x, float *y, float *w, float *h);

// Main tick function
void platform_tick(platform_t *p);

#endif // PLATFORM_H