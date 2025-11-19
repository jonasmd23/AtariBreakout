#ifndef BALL_H
#define BALL_H

#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"

// Ball structure
typedef struct {
    float x;              // x position
    float y;              // y position
    float dx;             // x velocity
    float dy;             // y velocity
    float radius;         // ball radius
    color_t color;        // ball color
    uint8_t currentState; // internal state machine state
    bool launch;          // flag to launch ball
} ball_t;

/************************ Initialization *************************/
// Initialize the ball to starting position and state
void ball_init(ball_t *ball);

/************************ Control Functions *************************/
// Launch the ball (start movement)
void ball_launch(ball_t *ball);

// Reset ball to starting position
void ball_reset(ball_t *ball);

// Move to next round with increased speed
void ball_next_round(ball_t *ball);

/************************ Status Functions *************************/
// Get ball position
void ball_get_pos(ball_t *ball, coord_t *x, coord_t *y);

// Check if ball is currently moving
bool ball_is_moving(ball_t *ball);

// Check if ball was lost (fell off bottom)
bool ball_is_lost(ball_t *ball);

/************************ Collision Functions *************************/
// Check and handle wall collisions
void ball_check_wall_collision(ball_t *ball);

// Check and handle platform collision
void ball_check_platform_collision(ball_t *ball,
                                   float px, float py,
                                   float pw, float ph);

// Check and handle brick collision
// Returns true if collision occurred
bool ball_check_brick_collision(ball_t *ball,
                                float bx, float by,
                                float bw, float bh);

/************************ Tick Function *************************/
// Update ball state machine (call every frame)
void ball_tick(ball_t *ball);

#endif // BALL_H