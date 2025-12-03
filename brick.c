#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "hw.h"
#include "lcd.h"
#include "brick.h"
#include "config.h"
#include "ball.h"        // Needed so we can bounce the ball

#define SCREEN_WIDTH LCD_W
#define SCREEN_HEIGHT LCD_H

// Brick internal states
enum brick_st_t {
    init_st,
    alive_st,
    dead_st
};

/************************ Single Brick Functions *************************/
static void brick_init_single(brick_t *b, float x, float y, float w, float h, color_t color) {
    if (!b) return;
    
    b->x = x;
    b->y = y;
    b->width = w;
    b->height = h;
    b->color = color;
    b->currentState = init_st;
    b->destroy_me = false;
}

static void brick_tick_single(brick_t *b) {
    if (!b) return;
    
    // ---------- State Transitions ----------
    switch (b->currentState) {
        case init_st:
            b->currentState = alive_st;
            break;
            
        case alive_st:
            if (b->destroy_me)
                b->currentState = dead_st;
            break;
            
        case dead_st:
            // Stay dead
            break;
    }
    
    // ---------- State Actions ----------
    switch (b->currentState) {
        case init_st:
            break;
            
        case alive_st:
            lcd_fillRect((coord_t)b->x, (coord_t)b->y,
                         (coord_t)b->width, (coord_t)b->height,
                         b->color);
            break;
            
        case dead_st:
            // Erase brick (only once)
            static bool erased = false;
            if (!erased) {
                lcd_fillRect((coord_t)b->x, (coord_t)b->y,
                             (coord_t)b->width, (coord_t)b->height,
                             CONFIG_COLOR_BACKGROUND);
                erased = true;
            }
            break;
    }
}

/************************ Brick Grid Functions *************************/
void bricks_init(brick_grid_t *grid) {
    if (!grid) return;
    
    grid->rows = 4;
    grid->cols = 10;
    grid->spacing_x = 6;
    grid->spacing_y = 6;

    float brick_width = 
        (SCREEN_WIDTH - (grid->cols + 1) * grid->spacing_x) / (float)grid->cols;
    float brick_height = 20;

    for (int r = 0; r < grid->rows; r++) {
        for (int c = 0; c < grid->cols; c++) {
            brick_t *b = &grid->bricks[r][c];
            
            float x = grid->spacing_x + c * (brick_width + grid->spacing_x);
            float y = grid->spacing_y + r * (brick_height + grid->spacing_y);
            
            color_t color;
            switch (r) {
                case 0: color = RED; break;
                case 1: color = WHITE; break;
                case 2: color = BLUE; break;
                case 3: color = RED; break;
                case 4: color = WHITE; break;
                default: color = WHITE; break;
            }
            
            brick_init_single(b, x, y, brick_width, brick_height, color);
        }
    }
}

void bricks_tick(brick_grid_t *grid) {
    if (!grid) return;
    
    for (int r = 0; r < grid->rows; r++)
        for (int c = 0; c < grid->cols; c++)
            brick_tick_single(&grid->bricks[r][c]);
}

/************************ Collision Detection With Bounce *************************/
bool bricks_check_collision(brick_grid_t *grid,
                            float ball_x, float ball_y, float ball_radius)
{
    if (!grid) return false;

    extern ball_t game_ball;   // Access the real ball to change dx/dy

    for (int r = 0; r < grid->rows; r++) {
        for (int c = 0; c < grid->cols; c++) {

            brick_t *b = &grid->bricks[r][c];

            // Skip dead or not fully alive yet
            if (b->currentState != alive_st) 
                continue;

            // ------- AABB-circle collision -------
            float closestX = ball_x;
            if (ball_x < b->x) closestX = b->x;
            else if (ball_x > b->x + b->width) closestX = b->x + b->width;

            float closestY = ball_y;
            if (ball_y < b->y) closestY = b->y;
            else if (ball_y > b->y + b->height) closestY = b->y + b->height;

            float dx = ball_x - closestX;
            float dy = ball_y - closestY;

            if ((dx*dx + dy*dy) <= (ball_radius * ball_radius)) {

                // ------- Bounce Logic -------
                float brick_center_x = b->x + b->width / 2.0f;
                float brick_center_y = b->y + b->height / 2.0f;

                float diff_x = ball_x - brick_center_x;
                float diff_y = ball_y - brick_center_y;

                // Decide bounce axis
                if (fabsf(diff_x) > fabsf(diff_y)) {
                    game_ball.dx *= -1;   // Horizontal bounce
                } else {
                    game_ball.dy *= -1;   // Vertical bounce
                }

                // Mark brick for removal
                b->destroy_me = true;

                return true;
            }
        }
    }

    return false;
}

bool bricks_all_cleared(brick_grid_t *grid) {
    if (!grid) return false;
    
    for (int r = 0; r < grid->rows; r++)
        for (int c = 0; c < grid->cols; c++)
            if (grid->bricks[r][c].currentState == alive_st)
                return false;

    return true;
}

uint32_t bricks_get_alive_count(brick_grid_t *grid) {
    if (!grid) return 0;
    
    uint32_t count = 0;
    for (int r = 0; r < grid->rows; r++)
        for (int c = 0; c < grid->cols; c++)
            if (grid->bricks[r][c].currentState == alive_st)
                count++;

    return count;
}
