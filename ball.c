#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "hw.h"
#include "lcd.h"
#include "ball.h"
#include "config.h"
#include "sound.h"

// Sound headers
#include "userSound.h"     // bounce sound
#include "missileLaunch.h" // lost sound

#define SCREEN_WIDTH LCD_W
#define SCREEN_HEIGHT LCD_H

#define SPEED 100
#define RADIUS 3

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
    ball->y = SCREEN_HEIGHT - 20;
    ball->dx = SPEED;          // horizontal speed
    ball->dy = -SPEED;         // vertical speed
    ball->radius = RADIUS;
    ball->color = WHITE;
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
    ball->y = SCREEN_HEIGHT - 20;
    ball->dx = SPEED;
    ball->dy = -SPEED;
    ball->currentState = idle_st;
    ball->launch = false;
}

void ball_next_round(ball_t *ball) {
    if (!ball) return;
    ball_init(ball);
    speed_multiplier *= 1.2f;   // increase speed by 20% each round
    if (speed_multiplier > 2.0f)
        speed_multiplier = 2.0f;
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

    bool bounced = false;

    if (ball->x - ball->radius <= 0) {
        ball->x = ball->radius;
        ball->dx *= -1;
        bounced = true;
    }
    if (ball->x + ball->radius >= SCREEN_WIDTH) {
        ball->x = SCREEN_WIDTH - ball->radius;
        ball->dx *= -1;
        bounced = true;
    }
    if (ball->y - ball->radius <= 0) {
        ball->y = ball->radius;
        ball->dy *= -1;
        bounced = true;
    }

    if (bounced)
        sound_start(userSound, USERSOUND_SAMPLES, false);
}

void ball_check_platform_collision(ball_t *ball,
                                   float px, float py,
                                   float pw, float ph) {
    if (!ball) return;

    if (ball->x + ball->radius >= px &&
        ball->x - ball->radius <= px + pw &&
        ball->y + ball->radius >= py &&
        ball->y - ball->radius <= py + ph &&
        ball->dy > 0)
    {
        ball->y = py - ball->radius;
        ball->dy *= -1;

        float hit_pos = (ball->x - px) / pw;
        float angle = (hit_pos - 0.5f) * 2.0f;
        ball->dx = angle * 200.0f;
        if (ball->dy > -100.0f)
            ball->dy = -100.0f;

        sound_start(userSound, USERSOUND_SAMPLES, false);
    }
}

bool ball_check_brick_collision(ball_t *ball,
                                float bx, float by,
                                float bw, float bh) {
    if (!ball) return false;

    float closestX = fmaxf(bx, fminf(ball->x, bx + bw));
    float closestY = fmaxf(by, fminf(ball->y, by + bh));

    float dx = ball->x - closestX;
    float dy = ball->y - closestY;

    const float epsilon = 0.01f;
    if ((dx*dx + dy*dy) <= (ball->radius * ball->radius + epsilon)) {

        float brick_center_x = bx + bw/2;
        float brick_center_y = by + bh/2;
        float diff_x = ball->x - brick_center_x;
        float diff_y = ball->y - brick_center_y;

        if (fabsf(diff_x) > fabsf(diff_y))
            ball->dx *= -1;
        else
            ball->dy *= -1;

        return true;  // collision detected
    }

    return false;
}



/************************ Tick Function *************************/
void ball_tick(ball_t *ball) {
    if (!ball) return;

    float dt = CONFIG_GAME_TIMER_PERIOD;

    switch (ball->currentState) {
        case init_st:  ball->currentState = idle_st; break;
        case idle_st:  if (ball->launch) ball->currentState = moving_st; break;
        case moving_st:
            ball->x += ball->dx * dt * speed_multiplier;
            ball->y += ball->dy * dt * speed_multiplier;

            ball_check_wall_collision(ball);
            // Make sure you check platform collision somewhere in main loop

            if (ball->y - ball->radius > SCREEN_HEIGHT) {
                ball->currentState = lost_st;
                sound_start(missileLaunch, MISSILELAUNCH_SAMPLES, false);
            }
            break;
        case lost_st: break;
    }

    // Draw ball
    switch (ball->currentState) {
        case init_st: break;
        case idle_st:
        case moving_st:
            lcd_fillCircle((coord_t)ball->x, (coord_t)ball->y,
                          (coord_t)ball->radius, ball->color);
            break;
        case lost_st:
            lcd_fillCircle((coord_t)ball->x, (coord_t)ball->y,
                          (coord_t)ball->radius, CONFIG_COLOR_BACKGROUND);
            break;
    }
}


