#ifndef BRICK_H
#define BRICK_H

#include <stdbool.h>
#include "lcd.h"

// One brick structure
typedef struct {
    float x, y;      // Top-left position
    float width, height;
    bool alive;      // True if brick not yet destroyed
    color_t color;
} Brick;

// Brick grid structure
typedef struct {
    Brick bricks[5][10]; // 5 rows, 10 columns
    int rows;
    int cols;
    float spacing_x;
    float spacing_y;
} BrickGrid;

// Initialize the brick grid
void bricks_init(BrickGrid *grid);

// Draw all bricks
void bricks_draw(BrickGrid *grid);

// Check if the ball collides with any brick
// Returns true if a brick was hit, and marks it dead
bool bricks_check_collision(BrickGrid *grid, float ball_x, float ball_y, float ball_radius);

// Returns true if all bricks are destroyed
bool bricks_all_cleared(BrickGrid *grid);

#endif
