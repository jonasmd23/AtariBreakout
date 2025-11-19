#include "ball.h"

#define SCREEN_WIDTH LCD_W
#define SCREEN_HEIGHT LCD_H
//initialize 
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
//upadte ball
void ball_update(Ball *ball, float dt)
{
    // Move ball
    ball->x += ball->dx * dt;
    ball->y += ball->dy * dt;

    // Check collisions
    ball_check_wall_collision(ball);

    // Check if ball fell past bottom
    if (ball->y - ball->radius > SCREEN_HEIGHT)
        ball->lost = true;
}
//wall
void ball_check_wall_collision(Ball *ball)
{
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
//platform
void ball_check_platform_collision(Ball *ball,
                                   float px, float py,
                                   float pw, float ph)
{
    // Simple AABB collision check
    if (ball->x + ball->radius >= px &&
        ball->x - ball->radius <= px + pw &&
        ball->y + ball->radius >= py &&
        ball->y - ball->radius <= py + ph)
    {
        // Move ball above platform to avoid sticking
        ball->y = py - ball->radius;
        ball->dy *= -1;

        // Adjust horizontal speed based on where it hits platform
        float hit_pos = (ball->x - px) / pw;  // 0 = left, 1 = right
        float angle = (hit_pos - 0.5f) * 2.0f; // -1 → 1

        ball->dx = angle * 150;
    }
}
//brick
bool ball_check_brick_collision(Ball *ball,
                                float bx, float by,
                                float bw, float bh)
{
    // Circle-rectangle collision (simple AABB)
    if (ball->x + ball->radius > bx &&
        ball->x - ball->radius < bx + bw &&
        ball->y + ball->radius > by &&
        ball->y - ball->radius < by + bh)
    {
        ball->dy *= -1;
        return true; // Brick hit
    }

    return false;
}
//ball
void ball_draw(Ball *ball)
{
    lcd_fillCircle((coord_t)ball->x, (coord_t)ball->y, (coord_t)ball->radius, ball->color);
}
