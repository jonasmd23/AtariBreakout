#ifndef BALL_H
#define BALL_H

#include <stdbool.h>
#include "lcd.h"  // for coord_t and color_t

typedef struct {
    float x, y;       // Ball position
    float dx, dy;     // Velocity
    float radius;     // Ball radius
    bool lost;        // True if ball fell below the platform
    color_t color;    // Ball color
} Ball;

// Initialize the ball in the center of the screen
void ball_init(Ball *ball);

// Update ball position; dt = time since last frame in seconds
void ball_update(Ball *ball, float dt);

// Check and handle collisions with screen walls
void ball_check_wall_collision(Ball *ball);

// Check collision with platform
void ball_check_platform_collision(Ball *ball,
                                   float px, float py,
                                   float pw, float ph);

// Check collision with a brick; return true if collision happened
bool ball_check_brick_collision(Ball *ball,
                                float bx, float by,
                                float bw, float bh);

// Draw the ball on the LCD
void ball_draw(Ball *ball);

#endif
