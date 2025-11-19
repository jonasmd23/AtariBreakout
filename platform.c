#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "hw.h"
#include "lcd.h"
#include "cursor.h"
#include "platform.h"
#include "config.h"
#include "joy.h"

#define SCREEN_WIDTH LCD_W
#define SCREEN_HEIGHT LCD_H

// Platform internal states
enum platform_st_t {
    init_st,
    active_st
};

/************************ Initialization *************************/
void platform_init(platform_t *p, uint32_t move_speed) {
    if (!p) return;
    
    p->width = 60;  // Platform width
    p->height = 10; // Platform height
    p->x = (SCREEN_WIDTH - p->width) / 2;
    p->y = SCREEN_HEIGHT - p->height - 5;  // 5 pixels from bottom
    p->color = BLUE;
    p->move_speed = move_speed;
    p->currentState = init_st;
    
    // Initialize joystick if not already done
    joy_init();
}

/************************ Control Functions *************************/
void platform_get_pos(platform_t *p, float *x, float *y, float *w, float *h) {
    if (!p) return;
    if (x) *x = p->x;
    if (y) *y = p->y;
    if (w) *w = p->width;
    if (h) *h = p->height;
}

/************************ Tick Function *************************/
void platform_tick(platform_t *p) {
    if (!p) return;
    
    float dt = CONFIG_GAME_TIMER_PERIOD;
    
    // ---------- State Transitions ----------
    switch (p->currentState) {
        case init_st:
            p->currentState = active_st;
            break;
            
        case active_st:
            // Stay in active state
            break;
            
        default:
            break;
    }
    
    // ---------- State Actions ----------
    switch (p->currentState) {
        case init_st:
            break;
            
        case active_st: {
            // Erase old position
            lcd_fillRect((coord_t)p->x, (coord_t)p->y,
                        (coord_t)p->width, (coord_t)p->height,
                        CONFIG_COLOR_BACKGROUND);
            
            // Get joystick input
            int32_t joy_x, joy_y;
            joy_get_displacement(&joy_x, &joy_y);
            
            // Convert to -1 to 1 proportion
            float joystick_proportion = (float)joy_x / JOY_MAX_DISP;
            
            // Move platform
            p->x += joystick_proportion * p->move_speed * dt;
            
            // Clamp to screen edges
            if (p->x < 0) 
                p->x = 0;
            if (p->x + p->width > SCREEN_WIDTH)
                p->x = SCREEN_WIDTH - p->width;
            
            // Draw new position
            lcd_fillRect((coord_t)p->x, (coord_t)p->y,
                        (coord_t)p->width, (coord_t)p->height,
                        p->color);
            break;
        }
        
        default:
            break;
    }
}