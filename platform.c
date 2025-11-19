#include "platform.h"

#define SCREEN_WIDTH LCD_W
#define SCREEN_HEIGHT LCD_H

void platform_init(Platform *p, float move_speed)
{
    p->width = 60;  // adjust as needed
    p->height = 10;
    p->x = (SCREEN_WIDTH - p->width) / 2;
    p->y = SCREEN_HEIGHT - p->height - 5;  // 5 pixels from bottom
    p->color = BLUE;
    p->move_speed = move_speed;

    // Initialize joystick if not already done
    joy_init();
}

void platform_update(Platform *p, float dt)
{
    int32_t joy_x, joy_y;
    joy_get_displacement(&joy_x, &joy_y);

    // Convert to -1 to 1 proportion
    float joystick_proportion = (float)joy_x / JOY_MAX_DISP;

    // Move platform
    p->x += joystick_proportion * p->move_speed * dt;

    // Clamp to screen edges
    if (p->x < 0) p->x = 0;
    if (p->x + p->width > SCREEN_WIDTH)
        p->x = SCREEN_WIDTH - p->width;
}

void platform_draw(Platform *p)
{
    lcd_fillRect((coord_t)p->x, (coord_t)p->y,
                 (coord_t)p->width, (coord_t)p->height,
                 p->color);
}
