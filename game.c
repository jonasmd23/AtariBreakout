#include <stdio.h>
#include <stdlib.h> // rand
#include <inttypes.h> // For PRIu32

#include "hw.h"
#include "lcd.h"
#include "cursor.h"
#include "sound.h"
#include "pin.h"
#include "ball.h"
#include "platform.h"
#include "brick.h"
#include "game.h"
#include "config.h"
// sound support
#include "missileLaunch.h"

#define PLAT_MOVE 300
#define SHOTS_X 10
#define STATS_Y 10

// Global game objects
ball_t game_ball;
platform_t game_platform;
brick_grid_t game_bricks;

static int total_bricks = 0;
static int broken_bricks = 0;

// Initialize game
void game_init(void)
{
    // Initialize game objects
    ball_init(&game_ball);
    platform_init(&game_platform, PLAT_MOVE); // 300 = move speed
    bricks_init(&game_bricks);
    
    // Initialize stats
    total_bricks = game_bricks.rows * game_bricks.cols;
    broken_bricks = 0;
    
    // Initialize joystick
    // joy_init();
}

// Main game tick function
void game_tick(void)
{
    // Get platform position for collision checking
    float plat_x, plat_y, plat_w, plat_h;
    platform_get_pos(&game_platform, &plat_x, &plat_y, &plat_w, &plat_h);
    
    // Check platform collision
    ball_check_platform_collision(&game_ball, plat_x, plat_y, plat_w, plat_h);
    
    // Check brick collisions
    coord_t ball_x, ball_y;
    ball_get_pos(&game_ball, &ball_x, &ball_y);
    
    if (bricks_check_collision(&game_bricks, 
                               (float)ball_x, (float)ball_y, 
                               (float)game_ball.radius)) {
        broken_bricks++;
        // Optional: play sound here
        // sound_start(missileLaunch, MISSILELAUNCH_LENGTH);
    }
    
    // Check if all bricks cleared (level complete)
    if (bricks_all_cleared(&game_bricks)) {
        ball_next_round(&game_ball);
        bricks_init(&game_bricks);
        total_bricks = game_bricks.rows * game_bricks.cols;
        broken_bricks = 0;
    }
    
    // Check if ball was lost
    if (ball_is_lost(&game_ball)) {
        // Reset ball for next attempt
        game_init();
    }
    
    // Launch ball with button press (BTN_A or BTN_START)
    if (!pin_get_level(HW_BTN_A) || !pin_get_level(HW_BTN_START)) {
        ball_launch(&game_ball);
    }
    
    // Update and draw everything (state machines handle their own drawing)
    platform_tick(&game_platform);
    bricks_tick(&game_bricks);
    ball_tick(&game_ball);
    
    // Draw stats
    char text_buffer[32];
    uint32_t remaining = bricks_get_alive_count(&game_bricks);
    snprintf(text_buffer, sizeof(text_buffer), "Bricks: %lu", 
             (unsigned long)remaining);
    lcd_drawString(SHOTS_X, STATS_Y, text_buffer, CONFIG_COLOR_STATUS);
}
