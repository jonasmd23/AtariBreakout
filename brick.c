#include "brick.h"

#define SCREEN_WIDTH LCD_W
#define SCREEN_HEIGHT LCD_H

void bricks_init(BrickGrid *grid)
{
    grid->rows = 5;
    grid->cols = 10;
    grid->spacing_x = 4; // space between bricks
    grid->spacing_y = 4;

    float brick_width = (SCREEN_WIDTH - (grid->cols + 1) * grid->spacing_x) / grid->cols;
    float brick_height = 12; // fixed height

    for (int r = 0; r < grid->rows; r++) {
        for (int c = 0; c < grid->cols; c++) {
            Brick *b = &grid->bricks[r][c];
            b->width = brick_width;
            b->height = brick_height;
            b->x = grid->spacing_x + c * (brick_width + grid->spacing_x);
            b->y = grid->spacing_y + r * (brick_height + grid->spacing_y);
            b->alive = true;

            // Color by row
            switch (r) {
                case 0: b->color = RED; break;
                case 1: b->color = ORANGE; break;
                case 2: b->color = YELLOW; break;
                case 3: b->color = GREEN; break;
                case 4: b->color = CYAN; break;
                default: b->color = WHITE; break;
            }
        }
    }
}

void bricks_draw(BrickGrid *grid)
{
    for (int r = 0; r < grid->rows; r++) {
        for (int c = 0; c < grid->cols; c++) {
            Brick *b = &grid->bricks[r][c];
            if (b->alive) {
                lcd_fillRect((coord_t)b->x, (coord_t)b->y,
                             (coord_t)b->width, (coord_t)b->height,
                             b->color);
            }
        }
    }
}

bool bricks_check_collision(BrickGrid *grid, float ball_x, float ball_y, float ball_radius)
{
    for (int r = 0; r < grid->rows; r++) {
        for (int c = 0; c < grid->cols; c++) {
            Brick *b = &grid->bricks[r][c];
            if (!b->alive) continue;

            // Simple AABB-circle collision
            float closestX = ball_x;
            if (ball_x < b->x) closestX = b->x;
            else if (ball_x > b->x + b->width) closestX = b->x + b->width;

            float closestY = ball_y;
            if (ball_y < b->y) closestY = b->y;
            else if (ball_y > b->y + b->height) closestY = b->y + b->height;

            float dx = ball_x - closestX;
            float dy = ball_y - closestY;

            if ((dx*dx + dy*dy) <= (ball_radius * ball_radius)) {
                b->alive = false;  // Mark brick as destroyed
                return true;
            }
        }
    }
    return false;
}

bool bricks_all_cleared(BrickGrid *grid)
{
    for (int r = 0; r < grid->rows; r++)
        for (int c = 0; c < grid->cols; c++)
            if (grid->bricks[r][c].alive) return false;

    return true;
}
