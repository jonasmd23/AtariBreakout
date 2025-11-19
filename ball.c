#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "hw.h"
#include "lcd.h"
#include "ball.h"
#include "config.h"

#define SCREEN_WIDTH LCD_W
#define SCREEN_HEIGHT LCD_H

#define SPEED 150
#define SIX 6

// Ball internal states
enum ball_st_t {
    init_st,
    idle_st,
    moving_st,
    lost_st
};

// Global speed multiplier
static float speed_multiplier = 1.0f;

/************************ Initialization *************************/
void ball_init(ball_t *ball) {
    if (!ball) return;
    
    ball->x = SCREEN_WIDTH / 2;
    ball->y = SCREEN_HEIGHT / 2;
    ball->dx = SPEED;          // horizontal speed
    ball->dy = -SPEED;         // vertical speed
    ball->radius = SIX;
    ball->color = RED;
    ball->currentState = init_st;
    ball->launch = false;
}

/************************ Control Functions *************************/
void ball_launch(ball_t *ball) {
    if (!ball) return;
    if (ball->currentState == idle_st)
        ball->launch = true;
}

void ball_reset(ball_t *ball) {
    if (!ball) return;
    ball->x = SCREEN_WIDTH / 2;
    ball->y = SCREEN_HEIGHT / 2;
    ball->dx = SPEED;
    ball->dy = -SPEED;  // Fixed: should be negative to go upward
    ball->currentState = idle_st;
    ball->launch = false;
}

void ball_next_round(ball_t *ball) {
    if (!ball) return;
    ball_init(ball);
    speed_multiplier *= 1.1f;   // increase speed by 10% each round
    if (speed_multiplier > 3.0f) 
        speed_multiplier = 3.0f; // cap max speed
}

/************************ Status Functions *************************/
void ball_get_pos(ball_t *ball, coord_t *x, coord_t *y) {
    if (!ball || !x || !y) return;
    *x = (coord_t)ball->x;
    *y = (coord_t)ball->y;
}

bool ball_is_moving(ball_t *ball) {
    return ball && (ball->currentState == moving_st);
}

bool ball_is_lost(ball_t *ball) {
    return ball && (ball->currentState == lost_st);
}

/************************ Collision Functions *************************/
void ball_check_wall_collision(ball_t *ball) {
    if (!ball) return;
    
    // Left wall
    if (ball->x - ball->radius <= 0) {
        ball->x = ball->radius;
        ball->dx *= -1;
    }
    
    // Right wall
    if (ball->x + ball->radius >= SCREEN_WIDTH) {
        ball->x = SCREEN_WIDTH - ball->radius;
        ball->dx *= -1;
    }
    
    // Top wall
    if (ball->y - ball->radius <= 0) {
        ball->y = ball->radius;
        ball->dy *= -1;
    }
}

void ball_check_platform_collision(ball_t *ball,
                                   float px, float py,
                                   float pw, float ph) {
    if (!ball) return;
    
    // Check if ball is within platform bounds
    if (ball->x + ball->radius >= px &&
        ball->x - ball->radius <= px + pw &&
        ball->y + ball->radius >= py &&
        ball->y - ball->radius <= py + ph &&
        ball->dy > 0)  // Only collide when moving down
    {
        ball->y = py - ball->radius; // move above platform
        ball->dy *= -1;

        // Adjust horizontal velocity based on where it hits
        float hit_pos = (ball->x - px) / pw;  // 0 = left, 1 = right
        float angle = (hit_pos - 0.5f) * 2.0f; // -1 → 1
        ball->dx = angle * 200.0f;
        
        // Ensure minimum vertical speed
        if (ball->dy > -100.0f)
            ball->dy = -100.0f;
    }
}

bool ball_check_brick_collision(ball_t *ball,
                                float bx, float by,
                                float bw, float bh) {
    if (!ball) return false;
    
    // Simple AABB-circle collision
    float closestX = ball->x;
    if (ball->x < bx) 
        closestX = bx;
    else if (ball->x > bx + bw) 
        closestX = bx + bw;

    float closestY = ball->y;
    if (ball->y < by) 
        closestY = by;
    else if (ball->y > by + bh) 
        closestY = by + bh;  // Fixed: was bx + bh

    float dx = ball->x - closestX;
    float dy = ball->y - closestY;

    if ((dx*dx + dy*dy) <= (ball->radius * ball->radius)) {
        // Determine bounce direction based on collision point
        float brick_center_x = bx + bw/2;
        float brick_center_y = by + bh/2;
        
        float diff_x = ball->x - brick_center_x;
        float diff_y = ball->y - brick_center_y;
        
        // Bounce more on dominant axis
        if (fabs(diff_x) > fabs(diff_y)) {
            ball->dx *= -1;
        } else {
            ball->dy *= -1;
        }
        
        return true;
    }
    return false;
}

/************************ Tick Function *************************/
void ball_tick(ball_t *ball) {
    if (!ball) return;
    
    float dt = CONFIG_GAME_TIMER_PERIOD;
    
    // ---------- State Transitions ----------
    switch (ball->currentState) {
        case init_st:
            ball->currentState = idle_st;
            break;
            
        case idle_st:
            if (ball->launch)
                ball->currentState = moving_st;
            break;
            
        case moving_st:
            // Check if ball fell below screen
            if (ball->y - ball->radius > SCREEN_HEIGHT)
                ball->currentState = lost_st;
            break;
            
        case lost_st:
            // Stay in lost state until reset
            break;
            
        default:
            break;
    }
    
    // ---------- State Actions ----------
    switch (ball->currentState) {
        case init_st:
            break;
            
        case idle_st:
            // Draw ball at starting position
            lcd_fillCircle((coord_t)ball->x, (coord_t)ball->y, 
                          (coord_t)ball->radius, ball->color);
            break;
            
        case moving_st:
            // Erase old position
            lcd_fillCircle((coord_t)ball->x, (coord_t)ball->y, 
                          (coord_t)ball->radius, CONFIG_COLOR_BACKGROUND);
            
            // Update position with speed multiplier
            ball->x += ball->dx * dt * speed_multiplier;
            ball->y += ball->dy * dt * speed_multiplier;
            
            // Check wall collisions
            ball_check_wall_collision(ball);
            
            // Draw new position
            lcd_fillCircle((coord_t)ball->x, (coord_t)ball->y, 
                          (coord_t)ball->radius, ball->color);
            break;
            
        case lost_st:
            // Erase ball
            lcd_fillCircle((coord_t)ball->x, (coord_t)ball->y, 
                          (coord_t)ball->radius, CONFIG_COLOR_BACKGROUND);
            break;
            
        default:
            break;
    }
}