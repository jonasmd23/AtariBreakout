#ifndef BRICK_H
#define BRICK_H

#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"

#define MAX_BRICK_ROWS 8
#define MAX_BRICK_COLS 12

// Single brick structure
typedef struct {
    float x;                    // X position
    float y;                    // Y position
    float width;                // Brick width
    float height;               // Brick height
    color_t color;              // Brick color
    uint32_t currentState;      // Current state
    bool destroy_me;            // Flag to destroy brick
} brick_t;

// Grid of bricks
typedef struct {
    brick_t bricks[MAX_BRICK_ROWS][MAX_BRICK_COLS];
    int rows;                   // Number of rows in use
    int cols;                   // Number of columns in use
    int spacing_x;              // Horizontal spacing
    int spacing_y;              // Vertical spacing
} brick_grid_t;

/************************ Function Prototypes *************************/

// Initialize brick grid
void bricks_init(brick_grid_t *grid);

// Main tick function for all bricks
void bricks_tick(brick_grid_t *grid);

// Check collision with ball
bool bricks_check_collision(brick_grid_t *grid, float ball_x, float ball_y, float ball_radius);

// Check if all bricks are cleared
bool bricks_all_cleared(brick_grid_t *grid);

// Get count of alive bricks
uint32_t bricks_get_alive_count(brick_grid_t *grid);

#endif // BRICK_H