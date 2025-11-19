#ifndef PLATFORM_H
#define PLATFORM_H

#include "lcd.h"
#include "joy.h"

// Platform structure
typedef struct {
    float x, y;        // Top-left position
    float width, height;
    color_t color;
    float move_speed;  // pixels per second
} Platform;

// Initialize platform at the bottom center of the screen
void platform_init(Platform *p, float move_speed);

// Update platform position based on joystick input and delta time
void platform_update(Platform *p, float dt);

// Draw platform on the screen
void platform_draw(Platform *p);

#endif
