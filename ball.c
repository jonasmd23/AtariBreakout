#include "ball.h"

#define SCREEN_WIDTH LCD_W
#define SCREEN_HEIGHT LCD_H

// Start multiplier at 1.0
float speed_multiplier = 1.0f;

void ball_init(Ball *ball)
{
    ball->x = SCREEN_WIDTH / 2;
    ball->y = SCREEN_HEIGHT / 2;
    ball->dx = 150;          // horizontal speed
    ball->dy = -150;         // vertical speed
    ball->radius = 6;
    ball->lost = false;
    ball->color = RED;
}

void ball_update(Ball *ball, float dt)
{
    // Move ball with speed multiplier
    ball->x += ball->dx * dt * speed_multiplier;
    ball->y += ball->dy * dt * speed_multiplier;

    // Check collisions with walls
    ball_check_wall_collision(ball);

    // Check if ball fell below the screen
    if (ball->y - ball->radius > SCREEN_HEIGHT)
        ball->lost = true;
}

void ball_check_wall_collision(Ball *ball)
{
    if (ball->x - ball->radius <= 0) {
        ball->x = ball->radius;
        ball->dx *= -1;
    }
    if (ball->x + ball->radius >= SCREEN_WIDTH) {
        ball->x = SCREEN_WIDTH - ball->radius;
        ball->dx *= -1;
    }
    if (ball->y - ball->radius <= 0) {
        ball->y = ball->radius;
        ball->dy *= -1;
    }
}

void ball_check_platform_collision(Ball *ball,
                                   float px, float py,
                                   float pw, float ph)
{
    if (ball->x + ball->radius >= px &&
        ball->x - ball->radius <= px + pw &&
        ball->y + ball->radius >= py &&
        ball->y - ball->radius <= py + ph)
    {
        ball->y = py - ball->radius; // move above platform
        ball->dy *= -1;

        // Adjust horizontal velocity based on where it hits
        float hit_pos = (ball->x - px) / pw;  // 0 = left, 1 = right
        float angle = (hit_pos - 0.5f) * 2.0f; // -1 → 1
        ball->dx = angle * 150;
    }
}

bool ball_check_brick_collision(Ball *ball,
                                float bx, float by,
                                float bw, float bh)
{
    if (ball->x + ball->radius > bx &&
        ball->x - ball->radius < bx + bw &&
        ball->y + ball->radius > by &&
        ball->y - ball->radius < by + bh)
    {
        ball->dy *= -1;
        return true;
    }
    return false;
}

void ball_draw(Ball *ball)
{
    lcd_fillCircle((coord_t)ball->x, (coord_t)ball->y, (coord_t)ball->radius, ball->color);
}

// Call this at the end of a round to reset the ball and increase speed
void ball_next_round(Ball *ball)
{
    ball_init(ball);
    speed_multiplier *= 1.1f;   // increase speed by 10% each round
    if (speed_multiplier > 3.0f) speed_multiplier = 3.0f; // cap max speed
}
